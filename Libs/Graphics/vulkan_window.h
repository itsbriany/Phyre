#pragma once
#include <vulkan.hpp>
#include "vulkan_swapchain.h"

namespace Phyre {
namespace Graphics {
class VulkanInstance;

class DeviceManager;
class VulkanWindow {
public:
    explicit VulkanWindow(uint32_t width, uint32_t height, const VulkanInstance& instance);
    
    // Clean up vulkan resources
    ~VulkanWindow();

    // A reference to the SurfaceKHR
    const vk::SurfaceKHR& GetSurfaceReference() const { return surface_; }

    // Getters
    const uint32_t& width() const { return width_; }
    const uint32_t& height() const { return height_; }

private:
    // Throws a runtime exception if the surface could not properly be initialized
    static vk::SurfaceKHR InitializeSurface(uint32_t width, uint32_t height, const vk::Instance& instance);

    // Destroys the SurfaceKHR
    void DestroySurface() const;

    // A reference to the vulkan instance
    const VulkanInstance& instance_;

    // Width of the window
    uint32_t width_;

    // Height of the window
    uint32_t height_;

    // The surface we are using to render images
    vk::SurfaceKHR surface_;

    // For logging
    static const std::string kWho;
};

}
}
