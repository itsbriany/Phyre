#pragma once
#include <vulkan.hpp>
#include <glm/glm.hpp>

namespace Phyre {
namespace Graphics {

// A read only buffer for shaders
class VulkanUniformBuffer {
public:
    VulkanUniformBuffer(const vk::Device& device);
    ~VulkanUniformBuffer();

    void set_memory(const vk::DeviceMemory& memory) { memory_ = memory; }
    const vk::Buffer& buffer() const { return buffer_; }
    const glm::mat4& mvp() const { return MVP_; }

private:
    static vk::Buffer InitializeBuffer(const vk::Device& device, const glm::mat4& MVP);

    const vk::Device& device_;
    glm::mat4 projection_;
    glm::mat4 view_;
    glm::mat4 model_;
    glm::mat4 clip_;
    glm::mat4 MVP_;
    vk::Buffer buffer_;
    vk::DeviceMemory memory_;

    // For logging
    static const std::string kWho;
};

}
}
