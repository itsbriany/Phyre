#include "vulkan_loader.h"
#include <GLFW/glfw3.h>
#include "vulkan_errors.h"

const char* Phyre::Graphics::VulkanLoader::kLunarGStandardValidation = "VK_LAYER_LUNARG_standard_validation";
const std::vector<const char*> Phyre::Graphics::VulkanLoader::kInstanceLayerNames = {
#ifndef NDEBUG
    kLunarGStandardValidation
#endif
};
const std::vector<const char*> Phyre::Graphics::VulkanLoader::kInstanceExtensionNames = {
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
const std::string Phyre::Graphics::VulkanLoader::kWho = "[VulkanLoader]";

// Initialization pipeline
Phyre::Graphics::VulkanLoader::VulkanLoader() : 
    instance_(LoadVulkanInstance()),
    p_debugger_(new VulkanDebugger(&instance_)),
    gpus_(LoadPhysicalDevices(instance_)),
    active_gpu_(gpus_.front()),
    p_window_(std::make_unique<VulkanWindow>(instance_)),
    p_device_(std::make_shared<VulkanDevice>(active_gpu_, p_window_->GetSurfaceReference())) {
    Logging::debug("Instantiated", kWho);
}

Phyre::Graphics::VulkanLoader::~VulkanLoader() {
    p_window_.reset();
    p_device_.reset();
    delete p_debugger_;
    instance_.destroy();
}

vk::Instance Phyre::Graphics::VulkanLoader::LoadVulkanInstance() {
    std::string error_message;
    if (!CheckValidationLayerSupport()) {
        error_message = "Requested validation layers, but could not find any";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    // Initialize the VkApplicationInfo structure
    const char ptr_application_name[] = "Phyre";
    uint32_t application_version = 1;
    const char ptr_engine_name[] = "Phyre::RenderingSystem";
    uint32_t engine_version = 1;
    uint32_t api_version = VK_API_VERSION_1_0;
    vk::ApplicationInfo application_info = vk::ApplicationInfo(ptr_application_name, application_version, ptr_engine_name, engine_version, api_version);

    // Initialize the VkInstanceCreateInfo structure
    vk::InstanceCreateInfo instance_create_info;
    instance_create_info.setPpEnabledLayerNames(kInstanceLayerNames.data());
    instance_create_info.setEnabledLayerCount(kInstanceLayerNames.size());
    instance_create_info.setPpEnabledExtensionNames(kInstanceExtensionNames.data());
    instance_create_info.setEnabledExtensionCount(kInstanceExtensionNames.size());
    instance_create_info.setPApplicationInfo(&application_info);
    
    
	// Are we successful in initializing the Vulkan instance?
    vk::Instance instance;
	vk::Result result = vk::createInstance(&instance_create_info, nullptr, &instance);
    if (ErrorCheck(result, kWho)) {
        return instance;
    }

    error_message = "Failed to create Vulkan instance";
    Logging::fatal(error_message, kWho);
    throw std::runtime_error(error_message);
}

Phyre::Graphics::VulkanLoader::PhysicalDeviceVector Phyre::Graphics::VulkanLoader::LoadPhysicalDevices(const vk::Instance& instance) {
	PhysicalDeviceVector gpus = instance.enumeratePhysicalDevices();
	for (const auto& result : gpus) {
		const vk::PhysicalDeviceProperties properties = result.getProperties();
		std::ostringstream oss;
        oss << "Found device: " << properties.deviceName;
		Logging::info(oss.str(), kWho);
	}
    if (gpus.empty()) {
        std::string error_message = "Could not locate any GPUs";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }
    return gpus;
}

bool Phyre::Graphics::VulkanLoader::CheckValidationLayerSupport() {
    // Figure out how many layer properties we need to allocate
    uint32_t layer_count;
    vk::enumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<vk::LayerProperties> available_layers_properties(layer_count);
    vk::enumerateInstanceLayerProperties(&layer_count, available_layers_properties.data());

    // The layer names available by our Vulkan SDK
    std::vector<const char*> available_layer_names(available_layers_properties.size());
    std::transform(available_layers_properties.begin(), available_layers_properties.end(), available_layer_names.begin(), [](const vk::LayerProperties& properties) {
        return properties.layerName;
    });

    std::vector<const char*> instance_layer_names = kInstanceLayerNames;
    std::sort(available_layer_names.begin(), available_layer_names.end());

    // The layer names we are providing
    std::sort(instance_layer_names.begin(), instance_layer_names.end());

    // Check if the layers we provided are a subset of the layers provided by the SDK
    return std::includes(available_layer_names.begin(), available_layer_names.end(),
                         instance_layer_names.begin(), instance_layer_names.end(), [](const char* available_layer, const char* using_layer) {
        return strcmp(available_layer, using_layer) == 0;
    });
}