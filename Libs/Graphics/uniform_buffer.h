#pragma once
#include <vulkan.hpp>
#include <glm/glm.hpp>
#include "vulkan_device.h"

namespace Phyre {
namespace Graphics {

// Vulkan parts that are associated with a uniform buffer object
class UniformBuffer {
public:
    // ----------------------- Construction/Destruction ---------------------
    UniformBuffer(const VulkanDevice& device, const vk::Buffer& buffer, const vk::DeviceMemory& memory, const vk::DescriptorBufferInfo& descriptor);
    ~UniformBuffer();

    // ----------------------- Interface ---------------------
    const vk::Buffer& buffer() const { return buffer_; }
    const vk::DeviceMemory& memory() const { return memory_; }
    const vk::DescriptorBufferInfo& descriptor() const { return descriptor_; }

private:
    // ----------------------- Data Members ---------------------
    // A reference to the vulkan device
    const VulkanDevice& device_;

    // The underlying buffer object
    vk::Buffer buffer_;

    // The memory mapped to this uniform buffer
    vk::DeviceMemory memory_;

    // The buffer descriptor used for updating this buffer
    vk::DescriptorBufferInfo descriptor_;

    // ----------------------- Logging Helper ---------------------
    static const std::string kWho;
};

// Group the model-view-projection 4D matrices
struct ModelViewProjection {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

}
}
