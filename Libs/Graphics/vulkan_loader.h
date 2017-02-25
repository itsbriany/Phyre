#pragma once
#include "vulkan_debugger.h"
#include "vulkan_window.h"
#include "device_manager.h"
#include "vulkan_gpu.h"
#include "vulkan_pipeline.h"

namespace Phyre {
namespace Graphics {

#ifdef NDEBUG
    const static bool kDebugging = false;
#else
    const static bool kDebugging = true;
#endif

class VulkanLoader {
public:
    // Layer identifiers
    static const char* kLunarGStandardValidation;

    // Type definitions
    typedef std::vector<VulkanGPU> GPUVector;
    typedef std::vector<vk::DeviceQueueCreateInfo> DeviceQueueCreateInfoVector;

    // Construction and destruction
    VulkanLoader();

    // Destroys all the allocated vulkan objects
    virtual ~VulkanLoader();

    // Get a reference to the Vulkan instance
    const vk::Instance& instance() const { return instance_; }


private:
    // Returns true if we have validation layer support
    static bool CheckValidationLayerSupport();
    static vk::Instance LoadVulkanInstance();
    static GPUVector LoadGPUs(const vk::Instance& instance);

    // Our debugger
    std::unique_ptr<VulkanDebugger> p_debugger_;

    // Our context
    vk::Instance instance_;

    // The Vulkan Physical devices
    GPUVector gpus_;

    // A reference to the Vulkan Physical device we are currently using
    const VulkanGPU& active_gpu_;

    // Where rendering operations involving a window are handled
    std::unique_ptr<VulkanWindow> p_window_;

    // Points to the logical device currently in use
    std::unique_ptr<DeviceManager> p_device_manager_;

    static const std::vector<const char*> kInstanceLayerNames;
    static const std::vector<const char*> kInstanceExtensionNames;

    // For logging purposes
    static const std::string kWho;
};

}
}
