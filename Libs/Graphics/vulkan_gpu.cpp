#include <Logging/logging.h>
#include "vulkan_gpu.h"

const std::string Phyre::Graphics::VulkanGPU::kWho = "[VulkanGPU]";

Phyre::Graphics::VulkanGPU::VulkanGPU(const vk::PhysicalDevice& physical_device) : 
    gpu_(physical_device),
    properties_(InitializePhysicalDeviceProperties(gpu_)),
    memory_properties_(InitializePhysicalDeviceMemoryProperties(gpu_)) {
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

Phyre::Graphics::VulkanGPU::~VulkanGPU() {
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

uint32_t Phyre::Graphics::VulkanGPU::FindMemoryTypeIndex(uint32_t memory_type_bits, vk::MemoryPropertyFlags requirements_mask) const {
    const vk::PhysicalDeviceMemoryProperties& memory_properties = memory_properties_;
    // Search memtypes to find first index with those properties
    for (uint32_t memory_type_index = 0; memory_type_index < memory_properties.memoryTypeCount; memory_type_index++) {
        if ((memory_type_bits & uint32_t(vk::MemoryPropertyFlagBits::eDeviceLocal)) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[memory_type_index].propertyFlags & requirements_mask) == requirements_mask) {
                return memory_type_index;
            }
        }
        memory_type_bits >>= 1;
    }
    // No memory types matched, return failure
    return UINT32_MAX;
}

vk::PhysicalDeviceProperties Phyre::Graphics::VulkanGPU::InitializePhysicalDeviceProperties(const vk::PhysicalDevice& physical_device) {
    vk::PhysicalDeviceProperties properties;
    physical_device.getProperties(&properties);
    return properties;
}

vk::PhysicalDeviceMemoryProperties Phyre::Graphics::VulkanGPU::InitializePhysicalDeviceMemoryProperties(const vk::PhysicalDevice& physical_device) {
    vk::PhysicalDeviceMemoryProperties memory_properties;
    physical_device.getMemoryProperties(&memory_properties);
    return memory_properties;
}


