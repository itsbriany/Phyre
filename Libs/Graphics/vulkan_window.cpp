#include <vulkan.hpp>
#include <Logging/logging.h>
#include <Input/os_window.h>

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
                           const VulkanGPU& gpu) :
    BaseClass(0),
    width_(width),
    height_(height),
    instance_(instance),
    gpu_(gpu),
    p_window_(nullptr)
{
    p_window_ = InitializeWindow(width, height, window_title);
    surface_ = InitializeSurface(instance_.get(), *p_window_);
    surface_present_modes_ = InitializePresentModes(gpu_, surface_);
    surface_formats_ = InitializeSurfaceFormats(gpu_, surface_);
    preferred_present_mode_ = InitializePreferredPresentMode(surface_present_modes_);
    preferred_surface_format_ = InitializePreferredSurfaceFormat(surface_formats_);

    PHYRE_LOG(trace, kWho) << "Instantiated";
}

VulkanWindow::Pointer VulkanWindow::Create(float width,
                                           float height,
                                           const std::string& window_title,
                                           const VulkanInstance& instance,
                                           const VulkanGPU& gpu) {
    Pointer p_vk_window = Pointer(new VulkanWindow(width, height, window_title, instance, gpu));
    p_vk_window->Bind(p_vk_window->window());
    return p_vk_window;
}

VulkanWindow::~VulkanWindow() {
    DestroySurface();
    Close();
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

void VulkanWindow::OnFramebufferResize(int width, int height) {
    width_ = static_cast<float>(width);
    height_ = static_cast<float>(height);
}

bool VulkanWindow::Update() const {
    return p_window_->Update();
}

void VulkanWindow::Close() const {
    p_window_->Close();
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



std::unique_ptr<Input::Window> VulkanWindow::InitializeWindow(float width, float height, const std::string& window_title) {
    return std::make_unique<Input::Window>(static_cast<int>(width), static_cast<int>(height), window_title);
}

vk::SurfaceKHR VulkanWindow::InitializeSurface(const vk::Instance& instance, const Input::Window& window) {
    // The surface where we render our output 
    // Underneath the covers, this calls the appropriate Vk<PLATFORM>SurfaceCreateInfoKHR
    return window.CreateVulkanSurfaceKHR(instance);
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

}
}