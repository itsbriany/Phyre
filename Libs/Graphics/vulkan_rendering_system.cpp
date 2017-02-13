#include <vulkan/vulkan.h>

#include "vulkan_rendering_system.h"
#include "logging.h"


Phyre::Graphics::VulkanRenderingSystem::VulkanRenderingSystem() {
    if (!InitializeVulkanInstance()) {
        Logging::fatal("Could not initialize Vulkan instance!", *this);
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
	const char* ptr_application_name = "Phyre";
	uint32_t application_version = 1;
	const char* ptr_engine_name = "Phyre::RenderingSystem";
	uint32_t engine_version = 1;
	uint32_t api_version = VK_API_VERSION_1_0;
	vk::ApplicationInfo application_info = vk::ApplicationInfo(ptr_application_name, application_version, ptr_engine_name, engine_version, api_version);

	// Initialize the VkInstanceCreateInfo structure
	vk::InstanceCreateInfo instance_create_info;
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
    physical_device_vector_ = results;
    return results.size();
}

std::pair<Phyre::Graphics::VulkanRenderingSystem::PtrPhysicalDevice, vk::DeviceQueueCreateInfo>
Phyre::Graphics::VulkanRenderingSystem::CheckGraphicsCapability() {
    /**
	 * Queues are categorized into families. We can think of families as GPU capabilities
	 * such as Graphics, Compute, performing pixel block copies (blits), etc...
	 * We can query the physical device to get a list of queues
	 * that only represent the queue families we are interested in.
	 */
    vk::DeviceQueueCreateInfo device_queue_create_info;
    if (physical_device_vector_.empty()) {
        Logging::error("Could not find any physical devices!", *this);
        return std::make_pair(nullptr, device_queue_create_info);
    }
    for (const auto& physical_device : physical_device_vector_) {
        std::vector<vk::QueueFamilyProperties> queue_family_properties_vector = physical_device.getQueueFamilyProperties();
        for (const auto& queue_family_properties : queue_family_properties_vector) {
            for (uint32_t queue_family_index = 0; queue_family_index < queue_family_properties.queueCount; ++queue_family_index) {
                if (queue_family_properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                    // Index the queues to point to the graphics family and balance the load between them
                    // by assigning them all the same queue priority level.
                    device_queue_create_info.setQueueFamilyIndex(queue_family_index);
                    device_queue_create_info.setQueueCount(queue_family_properties.queueCount);
                    device_queue_create_info.setPQueuePriorities(std::move(std::vector<float>(queue_family_properties.queueCount, 1.0f).data()));
                    return std::make_pair(std::make_shared<vk::PhysicalDevice>(physical_device), device_queue_create_info);
                }
            }
        }
    }
    return std::make_pair(nullptr, device_queue_create_info);
}

bool Phyre::Graphics::VulkanRenderingSystem::InitializeLogicalDevice() {
    std::pair<PtrPhysicalDevice, vk::DeviceQueueCreateInfo> device_queue_create_info_pair = CheckGraphicsCapability();
	if (!device_queue_create_info_pair.first) {
        Logging::error("Could not find a device which supports graphics", *this);
	    return false;
	}

    // For readability
    const vk::DeviceQueueCreateInfo& device_queue_create_info = device_queue_create_info_pair.second;
    const vk::PhysicalDevice& physical_device = *device_queue_create_info_pair.first;

    vk::DeviceCreateInfo device_create_info;
    device_create_info.setQueueCreateInfoCount(1); // We only use one queue create info
    device_create_info.setPQueueCreateInfos(&device_queue_create_info);
    
    vk::Result result = physical_device.createDevice(&device_create_info, nullptr, &device_);
    if (result == vk::Result::eSuccess) {
        return true;
    }
	    
    Logging::error("Could not create logical device", *this);
    return false;
}