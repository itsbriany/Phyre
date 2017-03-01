#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {

class VulkanGPU {
public:
    explicit VulkanGPU(const vk::PhysicalDevice& physical_device);
    ~VulkanGPU();
    const vk::PhysicalDevice& get() const { return gpu_; }
    const vk::PhysicalDeviceProperties& properties() const { return properties_; }
    const vk::PhysicalDeviceMemoryProperties& memory_properties() const { return memory_properties_; }
    
private:
    static vk::PhysicalDeviceProperties InitializePhysicalDeviceProperties(const vk::PhysicalDevice& physical_device);
    static vk::PhysicalDeviceMemoryProperties InitializePhysicalDeviceMemoryProperties(const vk::PhysicalDevice& physical_device);

    // The GPU
    const vk::PhysicalDevice gpu_;

    // The physical device's properties
    vk::PhysicalDeviceProperties properties_;

    // The physical device's memory properties
    vk::PhysicalDeviceMemoryProperties memory_properties_;

    // For logging
    static const std::string kWho;
};

}
}
