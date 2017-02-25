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

    // Returns the allocated device memory for the provided buffer
    VulkanUniformBuffer AllocateUniformBuffer() const;

    // Returns true if we can find a proper memory type from the gpu's memory properties
    bool CanFindMemoryTypeFromProperties(uint32_t type_bits, MemoryPropertyFlags requirements_mask, uint32_t& type_index) const;

private:
    const VulkanGPU& gpu_;
    const vk::Device& device_;
    const VulkanUniformBuffer& uniform_buffer_; // TODO: For testing only

    // For logging
    static const std::string kWho;
};

}
}
