#pragma once
#include <vulkan.hpp>
#include "vulkan_debugger.h"

namespace Phyre {
namespace Graphics {

class VulkanInstance {
public:
    VulkanInstance();

    // For cleaning up the callback
    ~VulkanInstance();

    // ------------------ Accessors  ------------------------
    const vk::Instance& get() const { return instance_; }

    // ---------------- Global Constants --------------------
    static const char* kLunarGStandardValidation;
    static const std::vector<const char*> kLayerNames;
    static const std::vector<const char*> kExtensionNames;

private:
    // ---------------- Initialization Process --------------
    void InitializeInstance();

    // --------------- Checks ------------------------------
    static bool HasLayerSupport();
    static bool HasExtensionSupport();
    // ---------------------- Data Members ------------------
    std::vector<vk::LayerProperties> layer_properties_vector_;
    std::vector<vk::ExtensionProperties> extension_properties_vector_;
    vk::Instance instance_;

    // For logging
    static const std::string kWho;
};

}
}
