#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkan_rendering_system.h"
#include "logging.h"


Phyre::Graphics::VulkanRenderingSystem::VulkanRenderingSystem() : p_active_physical_device_(nullptr) {
    if (!InitializeGLFW()) {
        Logging::fatal("Could not initialize GLFW!", *this);
        exit(EXIT_FAILURE);
    }
    if (!InitializeVulkanInstance()) {
        Logging::fatal("Could not initialize Vulkan instance!", *this);
        exit(EXIT_FAILURE);
    }
    if (!InitializeSurface()) {
        Logging::fatal("Could not initialize presentation surface!", *this);
        exit(EXIT_FAILURE);
    }
    if (!InitializePhysicalDevices()) {
        Logging::fatal("Could not initialize physical devices!", *this);
        exit(EXIT_FAILURE);
    }
    if (!InitializeLogicalDevice()) {
        Logging::fatal("Could not initialize logical devices!", *this);
        exit(EXIT_FAILURE);
    }
}

Phyre::Graphics::VulkanRenderingSystem::~VulkanRenderingSystem() {
    device_.destroy();
    vk_instance_.destroy();
}

bool
Phyre::Graphics::VulkanRenderingSystem::InitializeVulkanInstance() {
	// Initialize the VkApplicationInfo structure
	const char ptr_application_name[] = "Phyre";
	uint32_t application_version = 1;
	const char ptr_engine_name[] = "Phyre::RenderingSystem";
	uint32_t engine_version = 1;
	uint32_t api_version = VK_API_VERSION_1_0;
	vk::ApplicationInfo application_info = vk::ApplicationInfo(ptr_application_name, application_version, ptr_engine_name, engine_version, api_version);

	// Initialize the VkInstanceCreateInfo structure
	vk::InstanceCreateInfo instance_create_info;
    std::vector<const char*> extension_names(InitializeInstanceExtensionNames());
    instance_create_info.setPpEnabledExtensionNames(extension_names.data());
    instance_create_info.setEnabledExtensionCount(extension_names.size());
	instance_create_info.setPApplicationInfo(&application_info);

	// Are we successful in initializing the Vulkan instance?
	vk::Result result = vk::createInstance(&instance_create_info, nullptr, &vk_instance_);

	if (result == vk::Result::eSuccess) {
        return true;
	}
    std::string who = "[VulkanRenderingSystem::CreateVulkanInstance]";
	if (result == vk::Result::eErrorIncompatibleDriver) {
		Logging::fatal("Could not find a Vulkan compatible driver", *this);
		exit(EXIT_FAILURE);
	}
	Logging::fatal("Unknown error when creating vulkan instance", *this);
	exit(EXIT_FAILURE);
}

bool
Phyre::Graphics::VulkanRenderingSystem::InitializePhysicalDevices() {
	PhysicalDeviceVector results = vk_instance_.enumeratePhysicalDevices();
	for (const auto& result : results) {
		const vk::PhysicalDeviceProperties properties = result.getProperties();
		std::ostringstream oss;
        oss << "Found device: " << properties.deviceName;
		Logging::info(oss.str(), *this);
	}
    physical_devices_ = results;
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
    if (physical_devices_.empty()) {
        Logging::error("Could not find any physical devices!", *this);
        return device_queue_create_info;
    }
    for (const auto& physical_device : physical_devices_) {

        // Initialize a vector letting us know which queues currently support surface presentation
        std::vector<vk::Bool32> surface_support_vector(physical_device.getQueueFamilyProperties().size());
        for (uint32_t queue_family_index = 0; queue_family_index < physical_device.getQueueFamilyProperties().size(); ++queue_family_index) {
            physical_device.getSurfaceSupportKHR(queue_family_index, surface_, &surface_support_vector.data()[queue_family_index]);
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

bool Phyre::Graphics::VulkanRenderingSystem::InitializeLogicalDevice() {
    vk::DeviceQueueCreateInfo device_queue_create_info = InitializeSupportedQueueIndices();
	if (!p_active_physical_device_) {
        Logging::error("Could not find a device which supports graphics", *this);
	    return false;
	}

    vk::DeviceCreateInfo device_create_info;
    std::vector<const char*> extension_names(InitializeDeviceExtensionNames());
    device_create_info.setPpEnabledExtensionNames(extension_names.data());
    device_create_info.setEnabledExtensionCount(extension_names.size());
    device_create_info.setQueueCreateInfoCount(1); // We only use one queue create info
    device_create_info.setPQueueCreateInfos(&device_queue_create_info);
    
    vk::Result result = p_active_physical_device_->createDevice(&device_create_info, nullptr, &device_);
    if (result == vk::Result::eSuccess) {
        return true;
    }
	    
    Logging::error("Could not create logical device", *this);
    return false;
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

    std::ostringstream oss;
    oss << "Allocated " << command_buffers_.size() << " command buffers";
    Logging::debug(oss.str(), *this);
    return true;
}

bool
Phyre::Graphics::VulkanRenderingSystem::InitializeSurface() {
    // Cross platform window handle
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const char window_title[] = "Phyre";
    GLFWwindow* window = glfwCreateWindow(640, 480, window_title, nullptr, nullptr);

    // The surface where we render our output 
    // Underneath the covers, this calls the appropriate Vk<PLATFORM>SurfaceCreateInfoKHR
    // TODO: This surface must be destroyed before destrying the VkInstance
    VkSurfaceKHR surface;
    VkResult error = glfwCreateWindowSurface(vk_instance_, window, nullptr, &surface);
    if (error == VK_ERROR_EXTENSION_NOT_PRESENT) {
        Logging::error("Failed to instantiate vulkan surface: Instance WSI extensions not present", *this);
        return false;
    }
    if (error != VK_SUCCESS) {
        Logging::error("Failed to instantiate vulkan surface", *this);
        return false;
    }

    surface_ = surface;
    return true;
}

bool
Phyre::Graphics::VulkanRenderingSystem::InitializeGLFW() {
    return glfwInit();
}

std::vector<const char*>
Phyre::Graphics::VulkanRenderingSystem::InitializeInstanceExtensionNames() {
    return std::vector<const char*> {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif __ANDROID__
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#else 
      VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
    };    
}

std::vector<const char*> 
Phyre::Graphics::VulkanRenderingSystem::InitializeDeviceExtensionNames() {
    return std::vector<const char*> { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}
