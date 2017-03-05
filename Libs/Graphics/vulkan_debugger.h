#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {
class VulkanInstance;

class VulkanDebugger {
public:
    explicit VulkanDebugger(const VulkanInstance& instance);

    // For cleaning up the callback
    ~VulkanDebugger();

    // Let Vulkan know we want to receive reports from the debugging layer
    void InitializeDebugReport();
    
    // This will get invoked each time the vulkan debug layer calls back to us
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL Callback(VkDebugReportFlagsEXT flags,
                                                     VkDebugReportObjectTypeEXT objType,
                                                     uint64_t obj,
                                                     size_t location,
                                                     int32_t code,
                                                     const char* layerPrefix,
                                                     const char* msg,
                                                     void* userData);
private:
    // A reference to the instance so that we can clean up our callback
    const VulkanInstance& instance_;

    // The glue that Vulkan uses to call back to us
    vk::DebugReportCallbackEXT debug_report_callback_;

    // For logging
    static const std::string kWho;
};

}
}
