#pragma once
#include <vulkan.hpp>
#include "vulkan_gpu.h"
#include <iostream>

namespace Phyre {
namespace Graphics {
class VulkanWindow;
class VulkanMemoryManager;

class VulkanSwapchain {
public:
    struct SwapchainImage {
        vk::Image image;
        vk::ImageView image_view;
    };
    struct DepthImage {
        DepthImage(const vk::Image& image, const vk::ImageView& image_view, vk::Format format, const vk::DeviceMemory& device_memory) :
            image(image), image_view(image_view), format(format), device_memory(device_memory) {
            std::cout << "Depth image initialized" << std::endl;
        }
        ~DepthImage() { std::cout << "Depth image destoryed" << std::endl; }
        vk::Image image;
        vk::ImageView image_view;
        vk::Format format;
        vk::DeviceMemory device_memory;
    };

    typedef std::vector<SwapchainImage> SwapchainImageVector;

    explicit VulkanSwapchain(const VulkanMemoryManager& memory_manager,
                              const VulkanWindow& window,
                              const VulkanGPU& gpu,
                              const vk::Device& device, 
                              uint32_t graphics_queue_family_index,
                              uint32_t presentation_family_index);

    const SwapchainImageVector& swapchain_images() const { return swapchain_images_; }
    vk::Format image_format() const { return preferred_surface_format_.format; }
    vk::Format depth_format() const { return depth_image_.format; }
    vk::SampleCountFlagBits samples() const { return samples_; }
    uint32_t image_width() const { return image_width_; }
    uint32_t image_height() const { return image_height_; }
    const DepthImage& depth_image() const { return depth_image_; }
    const vk::SwapchainKHR& swapchain() const { return swapchain_; }

    ~VulkanSwapchain();

private:
    // --------------- Type definitions -----------------
    typedef std::vector<vk::PresentModeKHR> PresentModes;
    typedef std::vector<vk::ImageView> ImageViewVector;


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
    static vk::SwapchainKHR InitializeSwapchain(const vk::Device& device,
                                                const vk::SurfaceKHR& surface,
                                                const vk::SurfaceCapabilitiesKHR& surface_capabilities,
                                                const vk::SurfaceFormatKHR& surface_format,
                                                const vk::Extent2D& swapchain_extent,
                                                const vk::SurfaceTransformFlagBitsKHR& pre_transform,
                                                const vk::PresentModeKHR& surface_present_mode,
                                                uint32_t graphics_queue_family_index,
                                                uint32_t presentation_queue_family_index);

    // Throws a runtime exception if the swapchain images failed to instantiate
    static SwapchainImageVector InitializeSwapchainImages(const vk::Device& device, const vk::SwapchainKHR& swapchain, const vk::Format& format);

    // Throws a runtime exception if the depth buffer image failed to instantiate
    static DepthImage VulkanSwapchain::InitializeDepthImage(const VulkanMemoryManager& memory_manager, 
                                                             const VulkanGPU& gpu,
                                                             const vk::Device& device,
                                                             uint32_t width, 
                                                             uint32_t height,
                                                             vk::SampleCountFlagBits samples);

    // -------------------Data members -----------------

    // A reference to the memory manager
    const VulkanMemoryManager& memory_manager_;

    // A reference to the surface we want to send images to
    const vk::SurfaceKHR& surface_;
    
    // A reference to the window's width
    const uint32_t& image_width_;

    // A reference to the window's height
    const uint32_t& image_height_;

    // A reference to the GPU
    const VulkanGPU& gpu_;

    // A reference to the vulkan device
    const vk::Device& device_;

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

    // The images we swap
    SwapchainImageVector swapchain_images_;

    // How many image samples we are using
    vk::SampleCountFlagBits samples_;

    // The depth image
    DepthImage depth_image_;
};

}
}
