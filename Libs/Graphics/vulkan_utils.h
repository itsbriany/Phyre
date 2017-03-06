#pragma once
#include <vulkan.hpp>
#include "vulkan_gpu.h"

namespace Phyre {
namespace Graphics {

class VulkanUtils {
public:
    typedef vk::Flags<vk::MemoryPropertyFlagBits> MemoryPropertyFlags;

    // Returns true if we can find a proper memory type from the gpu's memory properties
    static bool CanFindMemoryTypeFromProperties(const VulkanGPU& gpu, uint32_t type_bits, MemoryPropertyFlags requirements_mask, uint32_t& type_index);

private:
    // For logging
    static const std::string kWho;
};

}
}
