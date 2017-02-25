#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {

class VulkanGPU {
public:
    explicit VulkanGPU(const vk::PhysicalDevice& physical_device);
    ~VulkanGPU();
    const vk::PhysicalDevice& physical_device() const { return physical_device_; }
    const vk::PhysicalDeviceProperties& properties() const { return properties_; }
    const vk::PhysicalDeviceMemoryProperties& memory_properties() const { return memory_properties_; }
    
private:
    static vk::PhysicalDeviceProperties InitializePhysicalDeviceProperties(const vk::PhysicalDevice& physical_device);
    static vk::PhysicalDeviceMemoryProperties InitializePhysicalDeviceMemoryProperties(const vk::PhysicalDevice& physical_device);

    // A reference to the physical device
    const vk::PhysicalDevice physical_device_;

    // The physical device's properties
    vk::PhysicalDeviceProperties properties_;

    // The physical device's memory properties
    vk::PhysicalDeviceMemoryProperties memory_properties_;

    // For logging
    static const std::string kWho;
};

}
}
