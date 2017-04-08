#pragma once
#include <vulkan.hpp>

namespace Phyre {
namespace Graphics {
class VulkanDevice;
class VulkanWindow;

class VulkanSwapchain {
public:
    struct SwapchainImage {
        vk::Image image;
        vk::ImageView image_view;
    };
    struct DepthImage {
        DepthImage(const vk::Image& image, const vk::ImageView& image_view, vk::Format format, const vk::DeviceMemory& device_memory) :
            image(image), image_view(image_view), format(format), device_memory(device_memory) { }
        vk::Image image;
        vk::ImageView image_view;
        vk::Format format;
        vk::DeviceMemory device_memory;
    };

    typedef std::vector<SwapchainImage> SwapchainImageVector;

    explicit VulkanSwapchain(const VulkanDevice& device, const VulkanWindow& window, VulkanSwapchain* p_old_swapchain);

    // ------------------------ Interface -----------------------------------
    
    // Wait for an image to become available and set the index of the image accordingly
    void AcquireNextImage();

    // ------------------------ Accessors -----------------------------------
    const VulkanWindow& window() const { return window_; }
    const SwapchainImageVector& swapchain_images() const { return swapchain_images_; }
    vk::Format depth_format() const { return depth_image_.format; }
    vk::SampleCountFlagBits samples() const { return samples_; }
    float image_width() const { return image_width_; }
    float image_height() const { return image_height_; }
    const DepthImage& depth_image() const { return depth_image_; }
    const vk::SwapchainKHR& swapchain() const { return swapchain_; }
    const vk::Semaphore& image_acquired_semaphore() const { return image_acquired_semaphore_; }
    uint32_t& current_frame_index() { return current_frame_index_; }

    ~VulkanSwapchain();

private:
    // --------------- Type definitions -----------------
    typedef std::vector<vk::PresentModeKHR> PresentModes;
    typedef std::vector<vk::ImageView> ImageViewVector;


    // --------------- Initializers ---------------------
    static const std::string kWho;
    
    // Returns the extent of the buffer we will render to
    // The width and height should be some default dimensions, but ideally, they should reflect those of an existing surface
    static vk::Extent2D InitializeSwapchainExtent(const VulkanWindow& window);
    
    // Returns the pre transform given the available surface capabilities
    static vk::SurfaceTransformFlagBitsKHR InitializePreTransform(const vk::SurfaceCapabilitiesKHR& surface_capabilities);
    
    // Throws a runtime exception if the swapchain failed to initialize
    static vk::SwapchainKHR InitializeSwapchain(const VulkanDevice& device,
                                                const VulkanWindow& window,
                                                const vk::Extent2D& extent,
                                                const vk::SurfaceTransformFlagBitsKHR& pre_transform,
                                                VulkanSwapchain* p_old_swapchain);

    // Throws a runtime exception if the swapchain images failed to instantiate
    static SwapchainImageVector InitializeSwapchainImages(const vk::Device& device, const vk::SwapchainKHR& swapchain, const vk::Format& format);

    // Throws a runtime exception if the depth buffer image failed to instantiate
    static DepthImage InitializeDepthImage(const VulkanDevice& device,
                                           float width,
                                           float height,
                                           vk::SampleCountFlagBits samples);

    static vk::Semaphore LoadImageAcquiredSemaphore(const VulkanDevice& device);

    // -------------------Data members -----------------
    // A reference to our window
    const VulkanWindow& window_;

    // A reference to the surface we want to send images to
    const vk::SurfaceKHR& surface_;
    
    // A reference to the window's width
    const float& image_width_;

    // A reference to the window's height
    const float& image_height_;

    // A reference to the vulkan device
    const VulkanDevice& device_;

    // The size of a rectangular retion of pixels within an image or framebuffer
    vk::Extent2D swapchain_extent_;

    // The transform relative to the presentation engine's natural orientation
    vk::SurfaceTransformFlagBitsKHR pre_transform_;

    // The underlying vulkan swapchain
    vk::SwapchainKHR swapchain_;

    // The images we swap
    SwapchainImageVector swapchain_images_;

    // How many image samples we are using
    vk::SampleCountFlagBits samples_;

    // The depth image
    DepthImage depth_image_;

    // We need to get an image from the swapchain in order to draw anything
    // Semaphores are used to ensure that all images in the swapchain are currently available
    vk::Semaphore image_acquired_semaphore_;

    // The index of the swapchain image which is currently available for rendering
    uint32_t current_frame_index_;
};

}
}
