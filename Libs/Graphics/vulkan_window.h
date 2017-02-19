#pragma once
#include <vulkan.hpp>
#include "loggable_interface.h"

namespace Phyre {
namespace Graphics {

class VulkanWindow : public Logging::LoggableInterface {
public:
    explicit VulkanWindow(const vk::Instance& instance);

    // Destroys the SurfaceKHR
    void DestroySurface();

    // Returns true if the SurfaceKHR was properly initialized
    bool InitializeSurface();

    // Returns true if the swapchain was properly initialized
    bool InitializeSwapchain();

    // A reference to the SurfaceKHR
    const vk::SurfaceKHR& GetSurfaceReference() const { return surface_; }

    // LoggableInterface overrides
    std::string log() override {
        return "[VulkanWindow]";
    }

private:
    // A reference to the vulkan instance
    const vk::Instance& instance_;

    // The surface we are using to render images
    vk::SurfaceKHR surface_;
};

}
}
