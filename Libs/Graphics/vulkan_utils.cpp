#include "vulkan_utils.h"

namespace Phyre {
namespace Graphics {
namespace VulkanUtils {

bool CanFindMemoryTypeFromProperties(const VulkanGPU& gpu, uint32_t memory_type_bits, MemoryPropertyFlags requirements_mask, uint32_t& memory_type_index) {
    const vk::PhysicalDeviceMemoryProperties& memory_properties = gpu.memory_properties();
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((memory_type_bits & uint32_t(vk::MemoryPropertyFlagBits::eDeviceLocal)) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                memory_type_index = i;
                return true;
            }
        }
        memory_type_bits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

}
}
}

