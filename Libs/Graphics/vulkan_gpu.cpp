#include "vulkan_gpu.h"
#include "logging.h"

const std::string Phyre::Graphics::VulkanGPU::kWho = "[VulkanGPU]";

Phyre::Graphics::VulkanGPU::VulkanGPU(const vk::PhysicalDevice& physical_device) : 
    physical_device_(physical_device),
    properties_(InitializePhysicalDeviceProperties(physical_device_)),
    memory_properties_(InitializePhysicalDeviceMemoryProperties(physical_device_)) {
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


