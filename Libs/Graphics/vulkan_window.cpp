#include "vulkan_window.h"
#include "logging.h"
#include <GLFW/glfw3.h>

// Dynamically loaded functions
static PFN_vkDestroySurfaceKHR  s_destroy_surface_khr_ = nullptr;

Phyre::Graphics::VulkanWindow::VulkanWindow(const vk::Instance& instance) : instance_(instance) { }

void 
Phyre::Graphics::VulkanWindow::DestroySurface() {
    s_destroy_surface_khr_ = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance_, "vkDestroySurfaceKHR"));
    if (s_destroy_surface_khr_) {
        s_destroy_surface_khr_(instance_, surface_, nullptr);
    } else {
        Logging::warning("Could not delete surface", *this);
    }
}

bool 
Phyre::Graphics::VulkanWindow::InitializeSurface() {
    if (!glfwInit()) {
        const std::string error_message = "Could not initialize GLFW!";
        Logging::error(error_message, *this);
        return false;
    }

    // Cross platform window handle
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const char window_title[] = "Phyre";
    GLFWwindow* window = glfwCreateWindow(640, 480, window_title, nullptr, nullptr);

    // The surface where we render our output 
    // Underneath the covers, this calls the appropriate Vk<PLATFORM>SurfaceCreateInfoKHR
    VkSurfaceKHR surface;
    VkResult error = glfwCreateWindowSurface(instance_, window, nullptr, &surface);
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
Phyre::Graphics::VulkanWindow::InitializeSwapchain() {
    if (!surface_) {
        Logging::error("No surface to create swapchain for", *this);
        return false;
    }

    vk::SwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.setSurface(surface_);
    swapchain_create_info.setImageFormat(vk::Format::eB8G8R8A8Unorm);  // This pixel format will do for now
                                                                       //swapchain_create_info.setMinImageCount(); // TODO: Fill these
                                                                       //swapchain_create_info.setImageExtent();
                                                                       //swapchain_create_info.setPreTransform();
                                                                       //swapchain_create_info.setPresentMode();

    return true;
}