#pragma once
#include <vulkan.hpp>
#include <glm/glm.hpp>

namespace Phyre {
namespace Graphics {
class VulkanDevice;

// A read only buffer for shaders
class VulkanUniformBuffer {
public:
    VulkanUniformBuffer(const VulkanDevice& device);
    ~VulkanUniformBuffer();

    void set_memory(const vk::DeviceMemory& memory) { device_memory_ = memory; }
    void set_descriptor_buffer_info(const vk::DescriptorBufferInfo& info) { descriptor_buffer_info_ = info; }
    const vk::Buffer& buffer() const { return buffer_; }
    const vk::DescriptorBufferInfo& descriptor_buffer_info() const { return descriptor_buffer_info_;  }
    const glm::mat4& mvp() const { return MVP_; }

private:
    static vk::Buffer InitializeBuffer(const vk::Device& device, const glm::mat4& MVP);
    void Initialize();

    const VulkanDevice& device_;
    glm::mat4 projection_;
    glm::mat4 view_;
    glm::mat4 model_;
    glm::mat4 clip_;
    glm::mat4 MVP_;
    vk::Buffer buffer_;
    vk::DeviceMemory device_memory_;
    vk::DescriptorBufferInfo descriptor_buffer_info_;

    // For logging
    static const std::string kWho;
};

}
}
