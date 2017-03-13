#include <Logging/logging.h>
#include "vulkan_window.h"
#include "vulkan_instance.h"
#include "vulkan_gpu.h"
#include <GLFW/glfw3.h>

const std::string Phyre::Graphics::VulkanWindow::kWho = "[VulkanWindow]";

// Dynamically loaded functions
static PFN_vkDestroySurfaceKHR  s_destroy_surface_khr_ = nullptr;

Phyre::Graphics::VulkanWindow::VulkanWindow(float width, float height, const std::string& window_title, const VulkanInstance& instance, const VulkanGPU& gpu) :
    width_(width),
    height_(height),
    instance_(instance),
    gpu_(gpu),
    p_window_(InitializeWindow(width, height, window_title)),
    is_running_(true),
    surface_(InitializeSurface(p_window_, instance_.get())),
    surface_capabilities_(InitializeSurfaceCapabilities(gpu_, surface_)),
    surface_present_modes_(InitializePresentModes(gpu_, surface_)),
    surface_formats_(InitializeSurfaceFormats(gpu_, surface_)),
    preferred_present_mode_(InitializePreferredPresentMode(surface_present_modes_)),
    preferred_surface_format_(InitializePreferredSurfaceFormat(surface_formats_)) {
    InitializeCallbacks();
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

Phyre::Graphics::VulkanWindow::~VulkanWindow() {
    DestroySurface();
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

bool Phyre::Graphics::VulkanWindow::Update() {
    glfwPollEvents();
    if (!p_window_ || glfwWindowShouldClose(p_window_)) {
        Close();
    }
    return is_running_;
}

void Phyre::Graphics::VulkanWindow::Close() {
    is_running_ = false;
}

void Phyre::Graphics::VulkanWindow::DestroySurface() const {
    s_destroy_surface_khr_ = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance_.get(), "vkDestroySurfaceKHR"));
    if (s_destroy_surface_khr_) {
        s_destroy_surface_khr_(instance_.get(), surface_, nullptr);
    } else {
        PHYRE_LOG(warning, kWho) << "Could not delete surface";
    }
}

void Phyre::Graphics::VulkanWindow::OSFramebufferResizeCallback(OSWindow*, int width, int height) {
    PHYRE_LOG(info, kWho) << "Window dimensions: (" << width << "x" << height << ')';
}

Phyre::Graphics::VulkanWindow::OSWindow* 
Phyre::Graphics::VulkanWindow::InitializeWindow(float width, float height, const std::string& window_title) {
    if (!glfwInit()) {
        std::string error_message = "Could not initialize GLFW!";
        PHYRE_LOG(error, kWho) << error_message;
        throw std::runtime_error(error_message);
    }

    // Cross platform window handle
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), window_title.c_str(), nullptr, nullptr);
}

vk::SurfaceKHR Phyre::Graphics::VulkanWindow::InitializeSurface(OSWindow* window, const vk::Instance& instance) {
    std::string error_message;

    // The surface where we render our output 
    // Underneath the covers, this calls the appropriate Vk<PLATFORM>SurfaceCreateInfoKHR
    VkSurfaceKHR surface;
    VkResult error = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (error == VK_ERROR_EXTENSION_NOT_PRESENT) {
        error_message = "Failed to instantiate vulkan surface: Instance WSI extensions not present";
        PHYRE_LOG(error, kWho) << error_message;
        throw std::runtime_error(error_message);
    }
    if (error != VK_SUCCESS) {
        error_message = "Failed to instantiate vulkan surface";
        PHYRE_LOG(error, kWho) << error_message;
        throw std::runtime_error(error_message);
    }

    return surface;
}

vk::SurfaceCapabilitiesKHR Phyre::Graphics::VulkanWindow::InitializeSurfaceCapabilities(const VulkanGPU& gpu, const vk::SurfaceKHR& surface) {
    return gpu.get().getSurfaceCapabilitiesKHR(surface);
}

std::vector<vk::PresentModeKHR> Phyre::Graphics::VulkanWindow::InitializePresentModes(const VulkanGPU& gpu, const vk::SurfaceKHR& surface) {
    std::vector<vk::PresentModeKHR> present_modes = gpu.get().getSurfacePresentModesKHR(surface);
    if (present_modes.empty()) {
        PHYRE_LOG(warning, kWho) << "No present modes found for the surface on the active GPU";
    }
    return present_modes;
}

std::vector<vk::SurfaceFormatKHR> Phyre::Graphics::VulkanWindow::InitializeSurfaceFormats(const VulkanGPU& gpu, const vk::SurfaceKHR& surface) {
    std::vector<vk::SurfaceFormatKHR> surface_formats = gpu.get().getSurfaceFormatsKHR(surface);
    if (surface_formats.empty()) {
        PHYRE_LOG(warning, kWho) << "No surface formats available on GPU";
    }
    return surface_formats;
}

vk::PresentModeKHR Phyre::Graphics::VulkanWindow::InitializePreferredPresentMode(const std::vector<vk::PresentModeKHR>& surface_present_modes) {
    // Tears when the app misses, but does not tear when the app is fast enough
    if (std::find(surface_present_modes.cbegin(), surface_present_modes.cend(), vk::PresentModeKHR::eFifoRelaxed) != surface_present_modes.cend()) {
        return vk::PresentModeKHR::eFifoRelaxed;
    }

    // Tearning is never observed
    if (std::find(surface_present_modes.cbegin(), surface_present_modes.cend(), vk::PresentModeKHR::eFifo) != surface_present_modes.cend()) {
        return vk::PresentModeKHR::eFifo;
    }

    // Never tears
    if (std::find(surface_present_modes.cbegin(), surface_present_modes.cend(), vk::PresentModeKHR::eMailbox) != surface_present_modes.cend()) {
        return vk::PresentModeKHR::eMailbox;
    }

    // Tears because the presentation requests are immediately rendered
    return vk::PresentModeKHR::eImmediate;
}

vk::SurfaceFormatKHR Phyre::Graphics::VulkanWindow::InitializePreferredSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_formats) {
    if (surface_formats.empty()) {
        PHYRE_LOG(warning, kWho) << "No surface formats detected!";
    }

    // The first image format is the preferred one
    vk::SurfaceFormatKHR surface_format;
    const vk::Format default_image_format = vk::Format::eB8G8R8A8Unorm;
    if (surface_formats.size() == 1 && surface_formats.front().format == vk::Format::eUndefined) {
        surface_format.format = default_image_format;
    } else {
        surface_format.format = surface_formats.front().format;
    }

    surface_format.colorSpace = surface_formats.front().colorSpace;
    return surface_format;
}

void Phyre::Graphics::VulkanWindow::InitializeCallbacks() const {
    glfwSetFramebufferSizeCallback(p_window_, &OSFramebufferResizeCallback);
}
