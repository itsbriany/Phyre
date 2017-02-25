#include "vulkan_uniform_buffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "vulkan_errors.h"

const std::string Phyre::Graphics::VulkanUniformBuffer::kWho = "[VulkanUniformBuffer]";

Phyre::Graphics::VulkanUniformBuffer::VulkanUniformBuffer(const vk::Device& device) :
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
    MVP_(clip_*projection_*view_*model_),      // A 4x4 matrix
    buffer_(InitializeBuffer(device_, MVP_)) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanUniformBuffer::~VulkanUniformBuffer() {
    device_.destroyBuffer(buffer_);
    device_.freeMemory(memory_);
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
