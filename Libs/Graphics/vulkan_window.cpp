#include <vulkan.hpp>
#include <Logging/logging.h>

#include "input.h"
#include "vulkan_instance.h"
#include "vulkan_gpu.h"
#include "vulkan_window.h"


namespace Phyre {
namespace Graphics {

const std::string VulkanWindow::kWho = "[VulkanWindow]";

// Dynamically loaded functions
static PFN_vkDestroySurfaceKHR  s_destroy_surface_khr_ = nullptr;

VulkanWindow::VulkanWindow(float width,
                           float height,
                           const std::string& window_title,
                           const VulkanInstance& instance,
                           const VulkanGPU& gpu,
                           Application* p_application) :
    width_(width),
    height_(height),
    instance_(instance),
    gpu_(gpu),
    p_window_(InitializeWindow(width, height, window_title)),
    is_running_(true),
    surface_(InitializeSurface(p_window_, instance_.get())),
    surface_present_modes_(InitializePresentModes(gpu_, surface_)),
    surface_formats_(InitializeSurfaceFormats(gpu_, surface_)),
    preferred_present_mode_(InitializePreferredPresentMode(surface_present_modes_)),
    preferred_surface_format_(InitializePreferredSurfaceFormat(surface_formats_)),
    p_application_(p_application),
    cursor_(p_window_)
{
    InitializeCallbacks();
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

VulkanWindow::~VulkanWindow() {
    DestroySurface();
    DestroyOSWindow(p_window_);
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

bool VulkanWindow::Update() {
    glfwPollEvents();
    if (!p_window_ || glfwWindowShouldClose(p_window_)) {
        Close();
    }
    return is_running_;
}

void VulkanWindow::Close() {
    is_running_ = false;
}

void VulkanWindow::DestroySurface() const {
    s_destroy_surface_khr_ = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance_.get(), "vkDestroySurfaceKHR"));
    if (s_destroy_surface_khr_) {
        s_destroy_surface_khr_(instance_.get(), surface_, nullptr);
    }
    else {
        PHYRE_LOG(warning, kWho) << "Could not delete surface";
    }
}

void VulkanWindow::DestroyOSWindow(OSWindow* p_os_window) {
    glfwDestroyWindow(p_os_window);
}

void VulkanWindow::OSFramebufferResizeCallback(OSWindow* p_os_window, int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }

    VulkanWindow *p_window = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(p_os_window));

    p_window->set_width(static_cast<float>(width));
    p_window->set_height(static_cast<float>(height));
    Application *p_application = p_window->application();

    p_application->OnFramebufferResize(width, height);
}

void VulkanWindow::OSWindowMousePositionCallback(OSWindow* p_os_window, double x, double y) {
    Application *p_application = GetApplicationFromWindow(p_os_window);
    p_application->OnMousePositionUpdate(x, y);
}

void VulkanWindow::OSWindowKeyCallback(OSWindow* p_os_window, int key, int /*scancode*/, int action, int mods) {
    Application *p_application = GetApplicationFromWindow(p_os_window);
    if (action == Input::Action::kPressed) {
        p_application->OnKeyPress(static_cast<Input::Key>(key), mods);
    } else if (action == Input::Action::kReleased) {
        p_application->OnKeyRelease(static_cast<Input::Key>(key), mods);
    } else if (action == Input::Action::kHold) {
        p_application->OnKeyHold(static_cast<Input::Key>(key), mods);
    }
}

void VulkanWindow::OSMouseButtonCallback(OSWindow* p_os_window, int button, int action, int mods) {
    VulkanWindow *p_window = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(p_os_window));
    Application *p_application = p_window->application();
    switch (action) {
    case Input::Action::kPressed:
        p_application->OnMousePress(static_cast<Input::Mouse>(button), mods);
        break;
    case Input::Action::kReleased:
        p_application->OnMouseRelease(static_cast<Input::Mouse>(button), mods);
        break;
    default:
        PHYRE_LOG(error, kWho) << "Could not determine mouse action";
    }
}

Application* VulkanWindow::GetApplicationFromWindow(OSWindow* p_os_window) {
    VulkanWindow *p_window = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(p_os_window));
    return p_window->application();
}

OSWindow* VulkanWindow::InitializeWindow(float width, float height, const std::string& window_title) {
    if (!glfwInit()) {
        std::string error_message = "Could not initialize GLFW!";
        PHYRE_LOG(error, kWho) << error_message;
        throw std::runtime_error(error_message);
    }

    // Cross platform window handle
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), window_title.c_str(), nullptr, nullptr);
}

vk::SurfaceKHR VulkanWindow::InitializeSurface(OSWindow* window, const vk::Instance& instance) {
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

vk::SurfaceCapabilitiesKHR VulkanWindow::InitializeSurfaceCapabilities(const VulkanGPU& gpu, const vk::SurfaceKHR& surface) {
    return gpu.get().getSurfaceCapabilitiesKHR(surface);
}

std::vector<vk::PresentModeKHR> VulkanWindow::InitializePresentModes(const VulkanGPU& gpu, const vk::SurfaceKHR& surface) {
    std::vector<vk::PresentModeKHR> present_modes = gpu.get().getSurfacePresentModesKHR(surface);
    if (present_modes.empty()) {
        PHYRE_LOG(warning, kWho) << "No present modes found for the surface on the active GPU";
    }
    return present_modes;
}

std::vector<vk::SurfaceFormatKHR> VulkanWindow::InitializeSurfaceFormats(const VulkanGPU& gpu, const vk::SurfaceKHR& surface) {
    std::vector<vk::SurfaceFormatKHR> surface_formats = gpu.get().getSurfaceFormatsKHR(surface);
    if (surface_formats.empty()) {
        PHYRE_LOG(warning, kWho) << "No surface formats available on GPU";
    }
    return surface_formats;
}

vk::PresentModeKHR VulkanWindow::InitializePreferredPresentMode(const std::vector<vk::PresentModeKHR>& surface_present_modes) {
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

vk::SurfaceFormatKHR VulkanWindow::InitializePreferredSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_formats) {
    if (surface_formats.empty()) {
        PHYRE_LOG(warning, kWho) << "No surface formats detected!";
    }

    // The first image format is the preferred one
    vk::SurfaceFormatKHR surface_format;
    const vk::Format default_image_format = vk::Format::eB8G8R8A8Unorm;
    if (surface_formats.size() == 1 && surface_formats.front().format == vk::Format::eUndefined) {
        surface_format.format = default_image_format;
    }
    else {
        surface_format.format = surface_formats.front().format;
    }

    surface_format.colorSpace = surface_formats.front().colorSpace;
    return surface_format;
}

void VulkanWindow::InitializeCallbacks() {
    // Allow calling back to us
    glfwSetWindowUserPointer(p_window_, this);

    // The rest of the callbacks get set here
    glfwSetFramebufferSizeCallback(p_window_, &OSFramebufferResizeCallback);
    glfwSetCursorPosCallback(p_window_, &OSWindowMousePositionCallback);
    glfwSetKeyCallback(p_window_, &OSWindowKeyCallback);
    glfwSetMouseButtonCallback(p_window_, &OSMouseButtonCallback);
}

}
}