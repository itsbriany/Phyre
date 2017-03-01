#include "vulkan_rendering_system.h"
#include "logging.h"

const std::string Phyre::Graphics::VulkanRenderingSystem::kWho = "[VulkanRenderingSystem]";

Phyre::Graphics::VulkanRenderingSystem::VulkanRenderingSystem() : 
    instance_(),
    debugger_(instance_),
    p_active_gpu_(nullptr),
    p_window_(nullptr) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanRenderingSystem::~VulkanRenderingSystem() {
    delete p_window_;
    Logging::trace("Destroyed", kWho);
}

void Phyre::Graphics::VulkanRenderingSystem::Start() {
#ifndef NDEBUG
    StartDebugger();
#endif
    LoadGPUs();
    LoadWindow(640, 480);

    // TODO On other platforms, we may need to initialize a connection to the window

}

void Phyre::Graphics::VulkanRenderingSystem::StartDebugger() {
    debugger_.InitializeDebugReport();
}

void Phyre::Graphics::VulkanRenderingSystem::LoadGPUs() {
    gpus_.clear();
    std::vector<vk::PhysicalDevice> physical_device_vector = instance_.get().enumeratePhysicalDevices();
    for (const vk::PhysicalDevice& physical_device : physical_device_vector) {
        const vk::PhysicalDeviceProperties properties = physical_device.getProperties();
        std::ostringstream oss;
        oss << "Found device: " << properties.deviceName;
        Logging::info(oss.str(), kWho);
        gpus_.emplace_back(VulkanGPU(physical_device));
    }
    if (gpus_.empty()) {
        std::string error_message = "Could not locate any GPUs";
        Logging::fatal(error_message, kWho);
        throw std::runtime_error(error_message);
    }
    p_active_gpu_ = &gpus_[0];
}

void Phyre::Graphics::VulkanRenderingSystem::LoadWindow(uint32_t width, uint32_t height) {
    p_window_ = new VulkanWindow(width, height, instance_);
}
