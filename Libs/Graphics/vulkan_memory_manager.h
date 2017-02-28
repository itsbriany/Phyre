#pragma once
#include <vulkan.hpp>
#include "vulkan_uniform_buffer.h"

namespace Phyre {
namespace Graphics {

class VulkanGPU;
class VulkanMemoryManager {
public:
    typedef vk::Flags<vk::MemoryPropertyFlagBits> MemoryPropertyFlags;

    explicit VulkanMemoryManager(const VulkanGPU& gpu, const vk::Device& device);
    ~VulkanMemoryManager();

    const VulkanGPU& gpu() const { return gpu_; }
    const VulkanUniformBuffer& uniform_buffer() const { return uniform_buffer_; }

    // Returns the allocated device memory for the provided buffer
    // VulkanUniformBuffer AllocateUniformBuffer() const;

    // Returns true if we can find a proper memory type from the gpu's memory properties
    static bool CanFindMemoryTypeFromProperties(const VulkanGPU& gpu, uint32_t type_bits, MemoryPropertyFlags requirements_mask, uint32_t& type_index);

private:
    const VulkanGPU& gpu_;
    const vk::Device& device_;
    VulkanUniformBuffer uniform_buffer_; // TODO: For testing only

    // For logging
    static const std::string kWho;
};

}
}
