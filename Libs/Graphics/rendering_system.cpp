#include <vulkan/vulkan.h>

#include "rendering_system.h"
#include "logging.h"


Phyre::Graphics::RenderingSystem::RenderingSystem() : p_vk_instance_(CreateVulkanInstance()) {}
Phyre::Graphics::RenderingSystem::~RenderingSystem() { }

Phyre::Graphics::RenderingSystem::PtrVkInstance
Phyre::Graphics::RenderingSystem::CreateVulkanInstance() {
	// Initialize the VkApplicationInfo structure
	const char* ptr_application_name = "Phyre";
	uint32_t application_version = 1;
	const char* ptr_engine_name = "Phyre::RenderingSystem";
	uint32_t engine_version = 1;
	uint32_t api_version = VK_API_VERSION_1_0;
	std::unique_ptr<vk::ApplicationInfo> p_application_info = std::make_unique<vk::ApplicationInfo>(ptr_application_name, application_version, ptr_engine_name, engine_version, api_version);

	// Initialize the VkInstanceCreateInfo structure
	std::unique_ptr<vk::InstanceCreateInfo> p_instance_create_info = std::make_unique<vk::InstanceCreateInfo>();
	p_instance_create_info->setPApplicationInfo(p_application_info.get());

	// Are we successful in initializing the Vulkan instance?
    PtrVkInstance p_instance = std::make_shared<vk::Instance>();
	vk::Result result = vk::createInstance(p_instance_create_info.get(), nullptr, p_instance.get());
	
	if (result == vk::Result::eSuccess) {
		return p_instance;
	}
    std::string who = "[RenderingSystem::CreateVulkanInstance]";
	if (result == vk::Result::eErrorIncompatibleDriver) {
		Logging::fatal("Could not find a Vulkan compatible driver", who);
		exit(EXIT_FAILURE);
	}
	Logging::fatal("Unknown error when creating vulkan instance", who);
	exit(EXIT_FAILURE);
}

Phyre::Graphics::RenderingSystem::PhysicalDeviceVector
Phyre::Graphics::RenderingSystem::EnumeratePhysicalDevices() {
	PhysicalDeviceVector results = p_vk_instance_->enumeratePhysicalDevices();
	for (const auto& result : results) {
		const vk::PhysicalDeviceProperties properties = result.getProperties();
		std::ostringstream oss;
        oss << "Found device: " << properties.deviceName;
		Logging::info(oss.str(), *this);
	}
	return results;
}

vk::DeviceQueueCreateInfo
Phyre::Graphics::RenderingSystem::CheckGraphicsCapability() {
    /**
	 * Queues are categorized into families. We can think of families as GPU capabilities
	 * such as Graphics, Compute, performing pixel block copies (blits), etc...
	 * We can query the physical device to get a list of queues
	 * that only represent the queue families we are interested in.
	 */
    DeviceQueueCreateInfoVector device_queue_create_info_vector;
    PhysicalDeviceVector physical_device_vector = EnumeratePhysicalDevices();
    for (const auto& physical_device : physical_device_vector) {
        std::vector<vk::QueueFamilyProperties> queue_family_properties_vector = physical_device.getQueueFamilyProperties();
        for (const auto& queue_family_properties : queue_family_properties_vector) {
            for (uint32_t queue_family_index = 0; queue_family_index < queue_family_properties.queueCount; ++queue_family_index) {
                if (queue_family_properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                    // Index the queues to point to the graphics family and balance the load between them
                    // by assigning them all the same queue priority level.
                    return vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(),
                                                     queue_family_index,
                                                     queue_family_properties.queueCount,
                                                     std::vector<float>(queue_family_properties.queueCount, 1.0f).data());
                }
            }
        }
    }
    return vk::DeviceQueueCreateInfo();
}

Phyre::Graphics::RenderingSystem::PtrDevice
Phyre::Graphics::RenderingSystem::CreateDevice() {
    PtrDevice p_device = std::make_shared<vk::Device>();
    vk::DeviceQueueCreateInfo device_queue_create_info = CheckGraphicsCapability();
	if (device_queue_create_info.queueCount == 0) {
        Logging::error("Could not find a device which supports graphics", *this);
	    return p_device;
	}

    vk::DeviceCreateInfo device_create_info;
    device_create_info.setQueueCreateInfoCount(1); // We only use one queue create info
    device_create_info.setPQueueCreateInfos(&device_queue_create_info);
    
    // TODO: Find a better way then to enumerate devices again
    vk::Result result = EnumeratePhysicalDevices().front().createDevice(&device_create_info, nullptr, p_device.get());
    if (result == vk::Result::eSuccess) {
        return p_device;
    }
	    
    Logging::error("Could not create logical device", *this);
    return p_device;
}