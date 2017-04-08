#pragma once
#include <vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Phyre {
namespace Graphics {

class Application;
class VulkanGPU;
class VulkanInstance;
class VulkanDevice;
class VulkanWindow {
public:
    explicit VulkanWindow(float width, float height,
                          const std::string& window_title,
                          const VulkanInstance& instance,
                          const VulkanGPU& gpu,
                          Application* p_application);
    
    // Clean up vulkan resources
    ~VulkanWindow();

    // Let the window live and respond to events continuously
    // Returns false on exit
    bool Update();
    
    // Close the window
    void Close();

    // -------------------- Accessors -----------------------
    const float& width() const { return width_; }
    const float& height() const { return height_; }
    const vk::SurfaceKHR& surface() const { return surface_; }
    const vk::SurfaceCapabilitiesKHR& surface_capabilities() const { return surface_capabilities_; }
    const std::vector<vk::PresentModeKHR>& present_modes() const { return surface_present_modes_; }
    const std::vector<vk::SurfaceFormatKHR>& surface_formats() const { return surface_formats_; }
    const vk::SurfaceFormatKHR& preferred_surface_format() const { return preferred_surface_format_; }
    const vk::PresentModeKHR& preferred_present_mode() const { return preferred_present_mode_; }


private:
    // -------------------- Type Definitions --------------------
    typedef GLFWwindow OSWindow;

    // -------------------- Initializers --------------------
    static OSWindow* InitializeWindow(float width, float height, const std::string& window_title);

    // Throws a runtime exception if the surface could not properly be initialized
    static vk::SurfaceKHR InitializeSurface(OSWindow* window, const vk::Instance& instance);

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

    // -------------------- Post-Initializers --------------------
    
    /**
     * \brief Register the callbacks to the application
     */
    void InitializeCallbacks() const;

    // -------------------- Destroyers ----------------------
    // Destroys the SurfaceKHR
    void DestroySurface() const;

    // -------------------- Callbacks ----------------------
    static void OSFramebufferResizeCallback(OSWindow*, int width, int height);

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
    OSWindow* p_window_;

    // Is the window alive and running?
    bool is_running_;

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

    // A pointer to our application such that we can call back to it
    Application* p_application_;

    // ------------------ Logging --------------------------
    static const std::string kWho;
};

}
}
