#include "vulkan_uniform_buffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "vulkan_utils.h"
#include "vulkan_device.h"
#include "logging.h"

const std::string Phyre::Graphics::VulkanUniformBuffer::kWho = "[VulkanUniformBuffer]";

Phyre::Graphics::VulkanUniformBuffer::VulkanUniformBuffer(const VulkanDevice& device) :
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
    device_.get().destroyBuffer(buffer_);
    device_.get().freeMemory(device_memory_);
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

    return device.createBuffer(buffer_create_info);
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

    buffer_ = device_.get().createBuffer(buffer_create_info);

    // Allocate buffer memory
    vk::MemoryRequirements memory_requirements = device_.get().getBufferMemoryRequirements(buffer_);
    vk::MemoryAllocateInfo allocate_info;
    vk::MemoryPropertyFlags requirements_mask(vk::MemoryPropertyFlagBits::eHostVisible |  // Memory is accessible to host
                                          vk::MemoryPropertyFlagBits::eHostCoherent); // Writes to this memory are visible to both host and device
    allocate_info.setAllocationSize(memory_requirements.size);
    if (!VulkanUtils::CanFindMemoryTypeFromProperties(device_.gpu(), memory_requirements.memoryTypeBits, requirements_mask, allocate_info.memoryTypeIndex)) {
        Logging::fatal("Failed to satisfy physical device memory requirements for allocating buffer", kWho);
    }
    device_.get().allocateMemory(&allocate_info, nullptr, &device_memory_);

    // Temporarily give the CPU access to this memory
    vk::MemoryMapFlags flags;
    void* p_data;
    device_.get().mapMemory(device_memory_, 0, memory_requirements.size, flags, static_cast<void**>(&p_data));
    memcpy(p_data, &MVP_, sizeof(MVP_));

    // Immediately unmap the memory because the page tables are limited in size for memory visible to
    // both CPU and GPU
    device_.get().unmapMemory(device_memory_);
    device_.get().bindBufferMemory(buffer_, device_memory_, 0);

    // Initialize the descriptor buffer info
    descriptor_buffer_info_.setBuffer(buffer_);
    descriptor_buffer_info_.setOffset(0);
    descriptor_buffer_info_.setRange(sizeof(MVP_));
}
