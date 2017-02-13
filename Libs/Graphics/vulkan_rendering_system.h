#pragma once
#include <vulkan.hpp>
#include "rendering_system_interface.h"
#include "loggable_interface.h"

namespace Phyre {
namespace Graphics {

class VulkanRenderingSystem : public RenderingSystemInterface, public Logging::LoggableInterface {
public:
    // Type definitions
    typedef std::shared_ptr<vk::Instance> PtrVkInstance;
    typedef std::shared_ptr<vk::Device> PtrDevice;
    typedef std::shared_ptr<vk::PhysicalDevice> PtrPhysicalDevice;
    typedef std::vector<vk::PhysicalDevice> PhysicalDeviceVector;
    typedef std::vector<vk::DeviceQueueCreateInfo> DeviceQueueCreateInfoVector;

    // Construction and destruction
    VulkanRenderingSystem();

    // Destroys all the allocated vulkan objects
    virtual ~VulkanRenderingSystem();

    // Returns true if the vulkan instance was instantiated
    bool InitializeVulkanInstance();

    // Returns true if physical devices could be initialized
    bool InitializePhysicalDevices();

    // Returns a vk::DeviceQueueCreateInfo associated with its physical device
    std::pair<PtrPhysicalDevice, vk::DeviceQueueCreateInfo> CheckGraphicsCapability();

    // Returns true if a logical device could be created from the physical device
    bool InitializeLogicalDevice();

    // Loggable interface overrides
    std::string log() override {
        return "[RenderingSystem]";
    }

    // Get a reference to the Vulkan instance
    const vk::Instance& instance() const { return vk_instance_; }


private:
    // A pointer to the Vulkan instance
    vk::Instance vk_instance_;

    // The Vulkan Physical devices
    PhysicalDeviceVector physical_device_vector_;

    // The Vulkan logical device derived from one of the
    // VkInstance's physical devices
    vk::Device device_;
};

}
}
