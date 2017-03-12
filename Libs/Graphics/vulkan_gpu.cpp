#include <Logging/logging.h>
#include "vulkan_gpu.h"

const std::string Phyre::Graphics::VulkanGPU::kWho = "[VulkanGPU]";

Phyre::Graphics::VulkanGPU::VulkanGPU(const vk::PhysicalDevice& physical_device) : 
    gpu_(physical_device),
    properties_(InitializePhysicalDeviceProperties(gpu_)),
    memory_properties_(InitializePhysicalDeviceMemoryProperties(gpu_)) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanGPU::~VulkanGPU() {
    Logging::trace("Destroyed", kWho);
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


