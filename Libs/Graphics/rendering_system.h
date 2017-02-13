#pragma once
#include <vulkan.hpp>
#include "loggable_interface.h"

namespace Phyre {
namespace Graphics {

// TODO: RenderingSystem should be an interface.
// This should be called VulkanRenderingSystem
class RenderingSystem : public Logging::LoggableInterface {
public:
    // Type definitions
    typedef std::shared_ptr<vk::Instance> PtrVkInstance;
    typedef std::shared_ptr<vk::Device> PtrDevice;
    typedef std::vector<vk::PhysicalDevice> PhysicalDeviceVector;
    typedef std::vector<vk::DeviceQueueCreateInfo> DeviceQueueCreateInfoVector;

    // Construction and destruction
    RenderingSystem();

    // TODO: Memory is likely to be leaking since the hpp bindings to not provide RAII
    // Free the memory using the callback allocators
    virtual ~RenderingSystem();

    // Initialize a vulkan instance if possible
    static PtrVkInstance CreateVulkanInstance();

    // Returns a vector with available physical devices
    PhysicalDeviceVector EnumeratePhysicalDevices();

    // Returns true if the device is capable of computing graphics
    vk::DeviceQueueCreateInfo CheckGraphicsCapability();

    // TODO: Maximize use of the total number of graphics queues found
    // Returns the amount of GPUs found
    PtrDevice CreateDevice();

    // Loggable interface overrides
    std::string log() override {
        return "[RenderingSystem]";
    }

    // Get a reference to the Vulkan instance
    const vk::Instance& instance() const { return *p_vk_instance_; }


private:
    // A pointer to the Vulkan instance
    PtrVkInstance p_vk_instance_;
};

}
}
