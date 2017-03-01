#include "vulkan_instance.h"
#include "logging.h"

const std::string Phyre::Graphics::VulkanInstance::kWho = "[VulkanInstance]";

// TODO: In the future, we may want to load these from a file since there are many more
// layer features we can use
const char* Phyre::Graphics::VulkanInstance::kLunarGStandardValidation = "VK_LAYER_LUNARG_standard_validation";
const std::vector<const char*> Phyre::Graphics::VulkanInstance::kLayerNames = {
#ifndef NDEBUG
    kLunarGStandardValidation
#endif
};
const std::vector<const char*> Phyre::Graphics::VulkanInstance::kExtensionNames = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif __ANDROID__
    VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#else 
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif

#ifndef NDEBUG
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
};

Phyre::Graphics::VulkanInstance::VulkanInstance() {
    InitializeInstance();
    Logging::trace("Initialized", kWho);
}

Phyre::Graphics::VulkanInstance::~VulkanInstance() {
    instance_.destroy();
    Logging::trace("Destroyed", kWho);
}

void Phyre::Graphics::VulkanInstance::InitializeInstance() {
    if (!HasLayerSupport()) {
        Logging::fatal("Some layers are not supported", kWho);
    }
    if (!HasExtensionSupport()) {
        Logging::fatal("Some extensions are not supported", kWho);
    }

    vk::ApplicationInfo application_info;
    application_info.setPApplicationName("Phyre");
    application_info.setApplicationVersion(1);
    application_info.setApiVersion(VK_API_VERSION_1_0);
    application_info.setPEngineName("Phyre Vulkan Rendering Engine");
    application_info.setEngineVersion(1);

    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.setPApplicationInfo(&application_info);
    instance_create_info.setEnabledExtensionCount(kExtensionNames.size());
    instance_create_info.setPpEnabledExtensionNames(kExtensionNames.data());
    instance_create_info.setEnabledLayerCount(kLayerNames.size());
    instance_create_info.setPpEnabledLayerNames(kLayerNames.data());
    instance_ = vk::createInstance(instance_create_info);
}

bool Phyre::Graphics::VulkanInstance::HasLayerSupport() {
    std::vector<vk::LayerProperties> available_layer_properties = vk::enumerateInstanceLayerProperties();

    // The extension names available by our Vulkan SDK
    std::vector<std::string> available_layer_names(available_layer_properties.size());
    std::transform(available_layer_properties.begin(), available_layer_properties.end(), available_layer_names.begin(), [](const vk::LayerProperties& properties) {
        return properties.layerName;
    });

    std::vector<std::string> using_layer_names(kLayerNames.begin(), kLayerNames.end());
    for (const std::string& using_layer : using_layer_names) {
        if (std::find(available_layer_names.begin(), available_layer_names.end(), using_layer) == available_layer_names.end()) {
            return false;
        }
    }
    return true;
}

bool Phyre::Graphics::VulkanInstance::HasExtensionSupport() {
    std::vector<vk::ExtensionProperties> available_extension_properties = vk::enumerateInstanceExtensionProperties();

    // The extension names available by our Vulkan SDK
    std::vector<std::string> available_extension_names(available_extension_properties.size());
    std::transform(available_extension_properties.begin(), available_extension_properties.end(), available_extension_names.begin(), [](const vk::ExtensionProperties& properties) {
        return properties.extensionName;
    });

    std::vector<std::string> using_extension_names(kExtensionNames.begin(), kExtensionNames.end());
    for (const std::string& using_extension : using_extension_names) {
        if (std::find(available_extension_names.begin(), available_extension_names.end(), using_extension) == available_extension_names.end()) {
            return false;
        }
    }
    return true;
}