#include "vulkan_rendering_system.h"
#include <GLFW/glfw3.h>
#include "vulkan_errors.h"

Phyre::Graphics::VulkanRenderingSystem::VulkanRenderingSystem() : 
instance_extension_names_({
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif __ANDROID__
    VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#else 
    VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
}),
device_extension_names_({
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
}),
window_(instance_),
p_debugger_(new VulkanDebugger)
{
    if (!InitializeVulkanInstance()) {
        std::string error_message = "Could not initialize Vulkan instance!";
        Logging::fatal(error_message, *this);
        throw std::runtime_error(error_message);
    }
    Logging::debug("Initialized Vulkan instance", *this);
    if (!window_.InitializeSurface()) {
        std::string error_message = "Could not initialize presentation surface!";
        Logging::fatal(error_message, *this);
        throw std::runtime_error(error_message);
    }
    Logging::debug("Initialized Vulkan surface", *this);
    if (kDebugging) {
        InitializeDebugExtensionsAndLayers();
    }
    if (!InitializePhysicalDevices()) {
        std::string error_message = "Could not initialize physical devices!";
        Logging::fatal(error_message, *this);
        throw std::runtime_error(error_message);
    }
    Logging::debug("Initialized Vulkan physical devices", *this);
    if (!InitializeLogicalDevice()) {
        std::string error_message = "Could not initialize logical devices!";
        Logging::fatal(error_message, *this);
        throw std::runtime_error(error_message);
    }
    Logging::debug("Initialized Vulkan logical device", *this);
    if (!window_.InitializeSwapchain()) {
        std::string error_message = "Could not initialize swapchain!";
        Logging::fatal(error_message, *this);
        throw std::runtime_error(error_message);
    }
    Logging::debug("Initialized Vulkan swapchain", *this);
}

Phyre::Graphics::VulkanRenderingSystem::~VulkanRenderingSystem() {
    delete p_debugger_;
    window_.DestroySurface();
    device_.destroy();
    instance_.destroy();
}

bool
Phyre::Graphics::VulkanRenderingSystem::InitializeVulkanInstance() {
    if (kDebugging && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Requested validation layers, but could not find any");
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
    if (kDebugging) {
        InitializeDebugExtensionsAndLayers();
    }
    instance_create_info.setPpEnabledLayerNames(instance_layer_names_.data());
    instance_create_info.setEnabledLayerCount(instance_layer_names_.size());
    instance_create_info.setPpEnabledExtensionNames(instance_extension_names_.data());
    instance_create_info.setEnabledExtensionCount(instance_extension_names_.size());
    instance_create_info.setPApplicationInfo(&application_info);
    
    
	// Are we successful in initializing the Vulkan instance?
	vk::Result result = vk::createInstance(&instance_create_info, nullptr, &instance_);
    if (ErrorCheck(result, log())) {
        if (kDebugging) {
           return p_debugger_->InitializeDebugReport(&instance_);
        }
        return true;
    }
    return false;
}

bool
Phyre::Graphics::VulkanRenderingSystem::InitializePhysicalDevices() {
	PhysicalDeviceVector results = instance_.enumeratePhysicalDevices();
	for (const auto& result : results) {
		const vk::PhysicalDeviceProperties properties = result.getProperties();
		std::ostringstream oss;
        oss << "Found device: " << properties.deviceName;
		Logging::info(oss.str(), *this);
	}
    gpus_ = results;
    return results.size();
}

vk::DeviceQueueCreateInfo
Phyre::Graphics::VulkanRenderingSystem::InitializeSupportedQueueIndices() {
    /**
	 * Queues are categorized into families. We can think of families as GPU capabilities
	 * such as Graphics, Compute, performing pixel block copies (blits), etc...
	 * We can query the physical device to get a list of queues
	 * that only represent the queue families we are interested in.
	 */
    vk::DeviceQueueCreateInfo device_queue_create_info;
    if (gpus_.empty()) {
        Logging::error("Could not find any physical devices!", *this);
        return device_queue_create_info;
    }
    for (const auto& physical_device : gpus_) {

        // Initialize a vector letting us know which queues currently support surface presentation
        std::vector<vk::Bool32> surface_support_vector(physical_device.getQueueFamilyProperties().size());
        for (uint32_t queue_family_index = 0; queue_family_index < physical_device.getQueueFamilyProperties().size(); ++queue_family_index) {
            physical_device.getSurfaceSupportKHR(queue_family_index, window_.GetSurfaceReference(), &surface_support_vector.data()[queue_family_index]);
        }

        std::vector<vk::QueueFamilyProperties> queue_family_properties_vector = physical_device.getQueueFamilyProperties();
        for (const auto& queue_family_properties : queue_family_properties_vector) {
            for (uint32_t queue_index = 0; queue_index < queue_family_properties.queueCount; ++queue_index) {
                if (queue_family_properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                    // Index the queues to point to the graphics family and balance the load between them
                    // by assigning them all the same queue priority level.
                    graphics_queue_family_index_ = queue_index;
                    device_queue_create_info.setQueueFamilyIndex(queue_index);
                    device_queue_create_info.setQueueCount(queue_family_properties.queueCount);
                    device_queue_create_info.setPQueuePriorities(std::move(std::vector<float>(queue_family_properties.queueCount, 1.0f).data()));

                    if (queue_index < surface_support_vector.size() && surface_support_vector[queue_index]) {
                        presentation_queue_family_index_ = queue_index;
                        p_active_physical_device_ = &physical_device;
                        return device_queue_create_info;
                    }
                }
            }
        }

        // If we did not find a graphics queue index, we still need to give a valid presentation queue index
        for (uint32_t queue_index = 0; queue_index < surface_support_vector.size(); ++queue_index) {
            if (surface_support_vector[queue_index]) {
                presentation_queue_family_index_ = queue_index;
                return device_queue_create_info;
            }
        }

        p_active_physical_device_ = &physical_device;
    }
    return device_queue_create_info;
}

bool 
Phyre::Graphics::VulkanRenderingSystem::InitializeLogicalDevice() {
    vk::DeviceQueueCreateInfo device_queue_create_info = InitializeSupportedQueueIndices();
	if (!p_active_physical_device_) {
        Logging::error("Could not find a device which supports graphics", *this);
	    return false;
	}

    vk::DeviceCreateInfo device_create_info;
    uint32_t queue_create_info_count = 1U;
    device_create_info.setPpEnabledExtensionNames(device_extension_names_.data());
    device_create_info.setEnabledExtensionCount(device_extension_names_.size());
    device_create_info.setPpEnabledLayerNames(device_layer_names_.data());
    device_create_info.setEnabledLayerCount(device_layer_names_.size());
    device_create_info.setQueueCreateInfoCount(queue_create_info_count); // We only use one queue create info 
    device_create_info.setPQueueCreateInfos(&device_queue_create_info);
    
    vk::Result result = p_active_physical_device_->createDevice(&device_create_info, nullptr, &device_);
    return ErrorCheck(result, log());
}

bool 
Phyre::Graphics::VulkanRenderingSystem::InitializeCommandBuffers() {
    /**
    * Command buffers reside in command buffer pools.
    * This is necessary for allocating command buffers 
    * because memory is coarsly allocated in large chunks between the CPU and GPU.
    */
    vk::CommandPoolCreateInfo command_pool_create_info(vk::CommandPoolCreateFlags(), graphics_queue_family_index_);
    vk::CommandPool command_pool;
    vk::Result create_command_pool_result = device_.createCommandPool(&command_pool_create_info, nullptr, &command_pool);
    if (create_command_pool_result != vk::Result::eSuccess) {
        Logging::error("Could not initialize command pool from logical device", *this);
        return false;
    }

    // Allocate the command buffers
    uint32_t command_buffer_count = 1; // Only allocate one command buffer for now
    vk::CommandBufferAllocateInfo command_buffer_allocate_info(std::move(command_pool), vk::CommandBufferLevel::ePrimary, command_buffer_count);
    command_buffers_ = device_.allocateCommandBuffers(command_buffer_allocate_info);
    if (command_buffers_.empty()) {
        Logging::error("Failed to allocate command buffers", *this);
        return false;
    }

    return true;
}

bool 
Phyre::Graphics::VulkanRenderingSystem::CheckValidationLayerSupport() {
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
    std::sort(available_layer_names.begin(), available_layer_names.end());

    // The layer names we are providing
    std::sort(instance_layer_names_.begin(), instance_layer_names_.end());

    // Check if the layers we provided are a subset of the layers provided by the SDK
    return std::includes(available_layer_names.begin(), available_layer_names.end(),
                         instance_layer_names_.begin(), instance_layer_names_.end(), [](const char* available_layer, const char* using_layer) {
        return strcmp(available_layer, using_layer) == 0;
    });
}

void 
Phyre::Graphics::VulkanRenderingSystem::InitializeDebugExtensionsAndLayers() {
    const char* kLunarGStandardValidation = "VK_LAYER_LUNARG_standard_validation";
    instance_extension_names_.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    instance_layer_names_.push_back(kLunarGStandardValidation);
    device_layer_names_.push_back(kLunarGStandardValidation);
}