#pragma once
#include <vulkan.hpp>
#include "loggable_interface.h"

namespace Phyre {
namespace Graphics {
class VulkanDevice;

class VulkanWindow;
class VulkanSwapchain : public Logging::LoggableInterface {
public:
    explicit VulkanSwapchain(const VulkanWindow& window, const VulkanDevice& deivce);

    //--------------- LoggableInterface overrides ---------------
    std::string log() override {
        return kWho;
    }

private:
    // --------------- Type definitions -----------------
    typedef std::vector<vk::PresentModeKHR> PresentModes;


    // --------------- Initializers ---------------------
    static const std::string kWho;

    // Allocates memory for the total surface format count
    static std::vector<vk::SurfaceFormatKHR> InitializeSurfaceFormats(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu);

    // Returns the preferred surface format out of all of the ones available
    static vk::SurfaceFormatKHR InitializePreferredSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_formats);

    // Returns the available surface capabilities
    static vk::SurfaceCapabilitiesKHR InitializeSurfaceCapabilities(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu);

    // Returns the extent of the buffer we will render to
    // The width and height should be some default dimensions, but ideally, they should reflect those of an existing surface
    static vk::Extent2D InitializeSwapchainExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& surface_capabilities);
    
    // Returns the pre transform given the available surface capabilities
    static vk::SurfaceTransformFlagBitsKHR InitializePreTransform(const vk::SurfaceCapabilitiesKHR& surface_capabilities);
    
    // Returns the present modes available on the gpu
    static PresentModes InitializeSurfacePresentModes(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& gpu);

    // Returns the optimal presentaion mode given the ones available
    static vk::PresentModeKHR InitializePreferredPresentMode(const PresentModes& surface_present_modes);
    
    // Throws a runtime exception if the swapchain failed to initialize
    static vk::SwapchainKHR InitializeSwapchain(const VulkanDevice& device,
                                                const vk::SurfaceKHR& surface,
                                                const vk::SurfaceCapabilitiesKHR& surface_capabilities,
                                                const vk::SurfaceFormatKHR& surface_format,
                                                const vk::Extent2D& swapchain_extent,
                                                const vk::SurfaceTransformFlagBitsKHR& pre_transform,
                                                const vk::PresentModeKHR& surface_present_mode);


    // -------------------Data members -----------------

    // A reference to the surface we want to send images to
    const vk::SurfaceKHR& surface_;

    // A reference to the vulkan device abstraction
    const VulkanDevice& device_;

    // The surface formats that the gpu provides us with
    std::vector<vk::SurfaceFormatKHR> surface_formats_;

    // Our preferred surface format
    vk::SurfaceFormatKHR preferred_surface_format_;

    // What kind of wonderful magic can the surface do?
    vk::SurfaceCapabilitiesKHR surface_capabilities_;

    // The size of a rectangular retion of pixels within an image or framebuffer
    vk::Extent2D swapchain_extent_;

    // The transform relative to the presentation engine's natural orientation
    vk::SurfaceTransformFlagBitsKHR pre_transform_;

    // Presentation modes provided by the surface
    PresentModes surface_present_modes_;

    // Our preferred surface presentation mode
    vk::PresentModeKHR preferred_surface_present_mode_;

    // The underlying vulkan swapchain
    vk::SwapchainKHR swapchain_;
};

}
}
