#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {
class VulkanGPU;
class VulkanInstance;
class VulkanDevice;
class VulkanWindow {
public:
    explicit VulkanWindow(uint32_t width, uint32_t height, const VulkanInstance& instance, const VulkanGPU& gpu);
    
    // Clean up vulkan resources
    ~VulkanWindow();

    // -------------------- Accessors -----------------------
    const uint32_t& width() const { return width_; }
    const uint32_t& height() const { return height_; }
    const vk::SurfaceKHR& surface() const { return surface_; }
    const vk::SurfaceCapabilitiesKHR& surface_capabilities() const { return surface_capabilities_; }
    const std::vector<vk::PresentModeKHR>& present_modes() const { return surface_present_modes_; }
    const std::vector<vk::SurfaceFormatKHR>& surface_formats() const { return surface_formats_; }
    const vk::SurfaceFormatKHR& preferred_surface_format() const { return preferred_surface_format_; }
    const vk::PresentModeKHR& preferred_present_mode() const { return preferred_present_mode_; }

private:
    // -------------------- Initializers --------------------
    // Throws a runtime exception if the surface could not properly be initialized
    static vk::SurfaceKHR InitializeSurface(uint32_t width, uint32_t height, const vk::Instance& instance);

    // Initialize surface capabilities
    static vk::SurfaceCapabilitiesKHR InitializeSurfaceCapabilities(const VulkanGPU& gpu, const vk::SurfaceKHR& surface);

    // Initialize the surface present modes on the GPU
    static std::vector<vk::PresentModeKHR> InitializePresentModes(const VulkanGPU& gpu, const vk::SurfaceKHR& surface);

    // Initialize the available surface formats on the GPU
    static std::vector<vk::SurfaceFormatKHR> InitializeSurfaceFormats(const VulkanGPU& gpu, const vk::SurfaceKHR& surface);

    // Returns the optimal presentaion mode given the ones available
    static vk::PresentModeKHR InitializePreferredPresentMode(const std::vector<vk::PresentModeKHR>& surface_present_modes);

    // Returns the optimal surface format given the ones available
    static vk::SurfaceFormatKHR InitializePreferredSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_formats);

    // -------------------- Destroyers ----------------------
    // Destroys the SurfaceKHR
    void DestroySurface() const;

    // ------------------ Data Members ---------------------
    // Width of the window
    uint32_t width_;

    // Height of the window
    uint32_t height_;

    // A reference to the vulkan instance
    const VulkanInstance& instance_;

    // A reference to the vulkan gpu
    const VulkanGPU& gpu_;

    // The surface we are using to render images
    vk::SurfaceKHR surface_;

    // What the surface is capable of on the active GPU
    vk::SurfaceCapabilitiesKHR surface_capabilities_;

    // The presentation modes avaialble on the active GPU
    std::vector<vk::PresentModeKHR> surface_present_modes_;

    // The surface formats available on the active GPU
    std::vector<vk::SurfaceFormatKHR> surface_formats_;

    // The preferred present mode based on the GPU hardware.
    vk::PresentModeKHR preferred_present_mode_;

    // The preferred surface format based on the GPU hardware
    vk::SurfaceFormatKHR preferred_surface_format_;

    // ------------------ Logging --------------------------
    static const std::string kWho;
};

}
}
