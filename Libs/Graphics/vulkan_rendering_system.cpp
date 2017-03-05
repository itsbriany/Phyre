#include "vulkan_rendering_system.h"
#include "logging.h"

const std::string Phyre::Graphics::VulkanRenderingSystem::kWho = "[VulkanRenderingSystem]";

Phyre::Graphics::VulkanRenderingSystem::VulkanRenderingSystem() : 
    instance_(),
    debugger_(instance_),
    p_active_gpu_(nullptr),
    p_window_(nullptr),
    p_device_(nullptr),
    p_swapchain_(nullptr),
    p_pipeline_(nullptr) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanRenderingSystem::~VulkanRenderingSystem() {
    delete p_pipeline_;
    delete p_swapchain_;
    p_device_->get().destroyCommandPool(command_pool_);
    delete p_window_;
    delete p_device_;
    Logging::trace("Destroyed", kWho);
}

void Phyre::Graphics::VulkanRenderingSystem::Start() {
#ifndef NDEBUG
    StartDebugger();
#endif
    LoadGPUs();
    LoadWindow(640, 480);
    // TODO On other platforms, we may need to initialize a connection to the window

    LoadDevice();
    LoadCommandPool();
    LoadCommandBuffers();
    ExecuteBeginCommandBuffer(0);
    LoadSwapchain();
    LoadPipeline();
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
    if (!p_active_gpu_) {
        Logging::error("Failed to load window: No active GPU", kWho);
        return;
    }
    p_window_ = new VulkanWindow(width, height, instance_, *p_active_gpu_);
}

void Phyre::Graphics::VulkanRenderingSystem::LoadDevice() {
    if (!AreDependenciesValid()) {
        return;
    }
    p_device_ = new VulkanDevice(*p_active_gpu_, *p_window_);
}

void Phyre::Graphics::VulkanRenderingSystem::LoadCommandPool() {
    /**
    * Command buffers reside in command buffer pools.
    * This is necessary for allocating command buffers
    * because memory is coarsly allocated in large chunks between the CPU and GPU.
    */
    if (!p_device_) {
        Logging::error("Failed to load command pool: No Device", kWho);
        return;
    }

    vk::CommandPoolCreateInfo command_pool_create_info(vk::CommandPoolCreateFlags(), p_device_->graphics_queue_family_index());
    command_pool_ = p_device_->get().createCommandPool(command_pool_create_info);
}

void Phyre::Graphics::VulkanRenderingSystem::LoadCommandBuffers() {
    if (!p_device_) {
        Logging::error("Failed to load command buffers: No Device", kWho);
        return;
    }
    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    uint32_t command_buffer_count = 1;
    command_buffer_allocate_info.setCommandPool(command_pool_);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    command_buffer_allocate_info.setCommandBufferCount(command_buffer_count);
    
    command_buffers_ = p_device_->get().allocateCommandBuffers(command_buffer_allocate_info);
}

void Phyre::Graphics::VulkanRenderingSystem::ExecuteBeginCommandBuffer(size_t command_buffer_index) {
    if (command_buffer_index > command_buffers_.size()) {
        Logging::error("Failed to begin command buffer: index out of bounds", kWho);
        return;
    }

    vk::CommandBufferBeginInfo info;
    info.setPInheritanceInfo(nullptr);
    command_buffers_[command_buffer_index].begin(&info);
    Logging::trace("Command buffer begin", kWho);
}

void Phyre::Graphics::VulkanRenderingSystem::LoadSwapchain() {
    if (!AreDependenciesValid()) {
        return;
    }
    p_swapchain_ = new VulkanSwapchain(*p_device_, *p_window_);
}

void Phyre::Graphics::VulkanRenderingSystem::LoadPipeline() {
    if (!p_device_) {
        Logging::error("Failed to load pipeline: No active device", kWho);
        return;
    }
    if (!p_swapchain_) {
        Logging::error("Failed to load pipeline: No active swapchain", kWho);
        return;
    }
    p_pipeline_ = new VulkanPipeline(*p_device_, *p_swapchain_);
}

bool Phyre::Graphics::VulkanRenderingSystem::AreDependenciesValid() const {
    if (!p_active_gpu_) {
        Logging::error("Failed to load logical device: No active GPU", kWho);
        return false;
    }
    if (!p_window_) {
        Logging::error("Failed to load logical device: No Window", kWho);
        return false;
    }

    return true;
}
