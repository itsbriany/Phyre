#include "vulkan_window.h"
#include "vulkan_instance.h"
#include "logging.h"
#include <GLFW/glfw3.h>

const std::string Phyre::Graphics::VulkanWindow::kWho = "[VulkanWindow]";

// Dynamically loaded functions
static PFN_vkDestroySurfaceKHR  s_destroy_surface_khr_ = nullptr;

Phyre::Graphics::VulkanWindow::VulkanWindow(uint32_t width, uint32_t height, const VulkanInstance& instance) :
    instance_(instance),
    width_(width),
    height_(height),
    surface_(InitializeSurface(width_, height_, instance_.get())) {
    Logging::trace("Instantiated", kWho);
}

Phyre::Graphics::VulkanWindow::~VulkanWindow() {
    DestroySurface();
    Logging::trace("Destroyed", kWho);
}

void Phyre::Graphics::VulkanWindow::DestroySurface() const {
    s_destroy_surface_khr_ = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance_.get(), "vkDestroySurfaceKHR"));
    if (s_destroy_surface_khr_) {
        s_destroy_surface_khr_(instance_.get(), surface_, nullptr);
    } else {
        Logging::warning("Could not delete surface", kWho);
    }
}

vk::SurfaceKHR Phyre::Graphics::VulkanWindow::InitializeSurface(uint32_t width, uint32_t height, const vk::Instance& instance) {
    std::string error_message;
    if (!glfwInit()) {
        error_message = "Could not initialize GLFW!";
        Logging::error(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    // Cross platform window handle
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const char window_title[] = "Phyre";
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);

    // The surface where we render our output 
    // Underneath the covers, this calls the appropriate Vk<PLATFORM>SurfaceCreateInfoKHR
    VkSurfaceKHR surface;
    VkResult error = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (error == VK_ERROR_EXTENSION_NOT_PRESENT) {
        error_message = "Failed to instantiate vulkan surface: Instance WSI extensions not present";
        Logging::error(error_message, kWho);
        throw std::runtime_error(error_message);
    }
    if (error != VK_SUCCESS) {
        error_message = "Failed to instantiate vulkan surface";
        Logging::error(error_message, kWho);
        throw std::runtime_error(error_message);
    }

    return surface;
}