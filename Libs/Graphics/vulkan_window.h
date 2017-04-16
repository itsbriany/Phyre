#pragma once
#include <boost/utility.hpp>
#include <Input/os_window.h>
#include <Input/handler.h>


#include "vulkan_gpu.h"


namespace Phyre {
namespace Graphics {

class VulkanInstance;
class VulkanDevice;
class VulkanWindow : public Input::Handler {
public:
    // -------------------- Type Definitions --------------------
    typedef std::shared_ptr<VulkanWindow> Pointer;

    // -------------------- Construction/Destruction ------------
    static Pointer Create(float width,
                          float height,
                          const std::string& window_title,
                          const VulkanInstance& instance,
                          const VulkanGPU& gpu);
    
    // Clean up vulkan resources
    ~VulkanWindow();

    // -------------------- Input::Handler Overrides ------------

    void OnFramebufferResize(int width, int height) override;

    // -------------------- Interface ---------------------------
    
    // Let the window live and respond to events continuously
    // Returns false on exit
    bool Update() const;
    
    // Close the window
    void Close();

    // -------------------- Accessors -----------------------
    const float& width() const { return width_; }
    const float& height() const { return height_; }
    const vk::SurfaceKHR& surface() const { return surface_; }
    vk::SurfaceCapabilitiesKHR surface_capabilities() const { return gpu_.get().getSurfaceCapabilitiesKHR(surface_); }
    const std::vector<vk::PresentModeKHR>& present_modes() const { return surface_present_modes_; }
    const std::vector<vk::SurfaceFormatKHR>& surface_formats() const { return surface_formats_; }
    const vk::SurfaceFormatKHR& preferred_surface_format() const { return preferred_surface_format_; }
    const vk::PresentModeKHR& preferred_present_mode() const { return preferred_present_mode_; }
    Input::Window* window() const { return p_window_.get(); }

    // -------------------- Setters -------------------------
    void set_width(float width) { width_ = width; }
    void set_height(float height) { height_ = height; }

private:
    // -------------------- Type Definitions --------------------
    typedef Handler BaseClass;

    // -------------------- Construction ------------------------
    explicit VulkanWindow(float width,
        float height,
        const std::string& window_title,
        const VulkanInstance& instance,
        const VulkanGPU& gpu);

    // -------------------- Initializers --------------------
    static std::unique_ptr<Input::Window> InitializeWindow(float width, float height, const std::string& window_title);

    // Throws a runtime exception if the surface could not properly be initialized
    static vk::SurfaceKHR InitializeSurface(const vk::Instance& instance, const Input::Window& window);

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
    float width_;

    // Height of the window
    float height_;

    // A reference to the vulkan instance
    const VulkanInstance& instance_;

    // A reference to the vulkan gpu
    const VulkanGPU& gpu_;

    // The underlying window implementation provided by the operating system
    std::unique_ptr<Input::Window> p_window_;

    // The surface we are using to render images
    vk::SurfaceKHR surface_;

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
