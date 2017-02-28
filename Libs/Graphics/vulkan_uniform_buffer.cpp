#include "vulkan_uniform_buffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "vulkan_errors.h"
#include "vulkan_memory_manager.h"

const std::string Phyre::Graphics::VulkanUniformBuffer::kWho = "[VulkanUniformBuffer]";

Phyre::Graphics::VulkanUniformBuffer::VulkanUniformBuffer(const VulkanGPU& gpu,
                                                          const vk::Device& device) :
    gpu_(gpu),
    device_(device),
    projection_(glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f)),
    view_(glm::lookAt(
        glm::vec3(-5, 3, -10),   // The camera's location coordinates in world space
        glm::vec3(0, 0, 0),      // looks at the origin
        glm::vec3(0, -1, 0)      // Head is up (set to 0, -1 ,0 to look upside-down)
    )),
    model_(glm::mat4(1.0f)),
    clip_(glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,   // Vulkan clip space has inverted Y and half Z
                    0.0f, -1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.5f, 0.0f,
                    0.0f, 0.0f, 0.5f, 1.0f)),
    MVP_(clip_*projection_*view_*model_)      // A 4x4 matrix
{
    Initialize(); // Post-Initialization
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanUniformBuffer::~VulkanUniformBuffer() {
    device_.destroyBuffer(buffer_);
    device_.freeMemory(device_memory_);
    Logging::trace("Destroyed", kWho);
}

vk::Buffer Phyre::Graphics::VulkanUniformBuffer::InitializeBuffer(const vk::Device& device, const glm::mat4& MVP) {
    vk::BufferCreateInfo buffer_create_info;
    buffer_create_info.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
    buffer_create_info.setSize(sizeof(MVP));
    buffer_create_info.setQueueFamilyIndexCount(0);
    buffer_create_info.setPQueueFamilyIndices(nullptr);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);
    buffer_create_info.setFlags(vk::BufferCreateFlags());

    vk::Buffer buffer;
    vk::Result result = device.createBuffer(&buffer_create_info, nullptr, &buffer);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to initilize uniform buffer", kWho);
    }
    return buffer;
}

void Phyre::Graphics::VulkanUniformBuffer::Initialize() {
    // Allocate the buffer
    vk::BufferCreateInfo buffer_create_info;
    buffer_create_info.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
    buffer_create_info.setSize(sizeof(MVP_));
    buffer_create_info.setQueueFamilyIndexCount(0);
    buffer_create_info.setPQueueFamilyIndices(nullptr);
    buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);
    buffer_create_info.setFlags(vk::BufferCreateFlags());

    vk::Result result = device_.createBuffer(&buffer_create_info, nullptr, &buffer_);
    if (!ErrorCheck(result, kWho)) {
        Logging::fatal("Failed to initilize uniform buffer", kWho);
    }

    // Allocate buffer memory
    vk::MemoryRequirements memory_requirements = device_.getBufferMemoryRequirements(buffer_);
    vk::MemoryAllocateInfo allocate_info;
    vk::MemoryPropertyFlags requirements_mask(vk::MemoryPropertyFlagBits::eHostVisible |  // Memory is accessible to host
                                          vk::MemoryPropertyFlagBits::eHostCoherent); // Writes to this memory are visible to both host and device
    allocate_info.setAllocationSize(memory_requirements.size);
    if (!VulkanMemoryManager::CanFindMemoryTypeFromProperties(gpu_, memory_requirements.memoryTypeBits, requirements_mask, allocate_info.memoryTypeIndex)) {
        Logging::fatal("Failed to satisfy physical device memory requirements for allocating buffer", kWho);
    }
    device_.allocateMemory(&allocate_info, nullptr, &device_memory_);

    // Temporarily give the CPU access to this memory
    vk::MemoryMapFlags flags;
    void* p_data;
    device_.mapMemory(device_memory_, 0, memory_requirements.size, flags, static_cast<void**>(&p_data));
    memcpy(p_data, &MVP_, sizeof(MVP_));

    // Immediately unmap the memory because the page tables are limited in size for memory visible to
    // both CPU and GPU
    device_.unmapMemory(device_memory_);
    device_.bindBufferMemory(buffer_, device_memory_, 0);

    // Initialize the descriptor buffer info
    descriptor_buffer_info_.setBuffer(buffer_);
    descriptor_buffer_info_.setOffset(0);
    descriptor_buffer_info_.setRange(sizeof(MVP_));
}
