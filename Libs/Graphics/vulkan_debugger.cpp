#include "vulkan_debugger.h"
#include "vulkan_errors.h"

static PFN_vkCreateDebugReportCallbackEXT s_create_debug_report_callback_proxy = nullptr;
static PFN_vkDestroyDebugReportCallbackEXT s_destroy_debug_report_callback_proxy = nullptr;

// This function is declared on some vulkan header file, but we must define it.
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
    VkInstance                                  instance,
    const VkDebugReportCallbackCreateInfoEXT*   p_create_info_info,
    const VkAllocationCallbacks*                p_allocator,
    VkDebugReportCallbackEXT*                   p_callback) {
    s_create_debug_report_callback_proxy = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    s_destroy_debug_report_callback_proxy = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    if (s_create_debug_report_callback_proxy && s_destroy_debug_report_callback_proxy) {
        return s_create_debug_report_callback_proxy(instance, p_create_info_info, p_allocator, p_callback);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

const std::string Phyre::Graphics::VulkanDebugger::kWho = "[VulkanDebugger]";

Phyre::Graphics::VulkanDebugger::VulkanDebugger(const vk::Instance* instance) : p_vk_instance_(instance), debug_report_callback_(nullptr) {
    Logging::trace("Intantiated", kWho);
}

// Comment out this definition to see if the debugger works.
// Rationale: Vulkan validation layers will scream at you if you do not free the debug report callback.
Phyre::Graphics::VulkanDebugger::~VulkanDebugger() {
    if (s_destroy_debug_report_callback_proxy) {
        s_destroy_debug_report_callback_proxy(*p_vk_instance_, debug_report_callback_, nullptr);
    }
    Logging::trace("Destoryed", kWho);
}

bool Phyre::Graphics::VulkanDebugger::InitializeDebugReport(const vk::Instance* instance) {
    if (!instance) {
        Logging::error("Cannot initialize debug report when vulkan instance is nullptr", kWho);
        return false;
    }

    p_vk_instance_ = instance;
    vk::DebugReportCallbackCreateInfoEXT debug_report_info;
    debug_report_info.setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning);
    debug_report_info.setPfnCallback(Callback);
    debug_report_info.pUserData = reinterpret_cast<void *>(this);

    vk::Result result = instance->createDebugReportCallbackEXT(&debug_report_info, nullptr, &debug_report_callback_);
    return ErrorCheck(result, kWho);
}

vk::Bool32 Phyre::Graphics::VulkanDebugger::Callback(VkDebugReportFlagsEXT flags,
                                                     VkDebugReportObjectTypeEXT object_type,
                                                     uint64_t obj,
                                                     size_t location,
                                                     int32_t code,
                                                     const char* layer_prefix,
                                                     const char* message,
                                                     void* userData) {
    std::ostringstream who_stream;
    who_stream << "[Vulkan::Validation]" << '[' << layer_prefix << ']';
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        Logging::info(message, who_stream.str());
    }
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        Logging::warning(message, who_stream.str());
    }
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        std::ostringstream oss;
        oss << "(Performnce warning) " << message;
        Logging::warning(oss.str(), who_stream.str());
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        Logging::error(message, who_stream.str());
    }
    return false;
}
