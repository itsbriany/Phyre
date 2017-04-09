#pragma once
#include <GLFW/glfw3.h>

#include "vulkan_gpu.h"
#include "application.h"
#include "cursor.h"

namespace Phyre {
namespace Graphics {

class VulkanInstance;
class VulkanDevice;
class VulkanWindow {
public:
    // -------------------- Construction/Destruction ------------
    explicit VulkanWindow(float width, float height,
                          const std::string& window_title,
                          const VulkanInstance& instance,
                          const VulkanGPU& gpu,
                          Application* p_application);
    
    // Clean up vulkan resources
    ~VulkanWindow();

    // -------------------- Interface ---------------------------

    // Let the window live and respond to events continuously
    // Returns false on exit
    bool Update();
    
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
    Application* application() const { return p_application_; }
    Cursor& cursor() { return cursor_; }

    // -------------------- Setters -------------------------
    void set_width(float width) { width_ = width; }
    void set_height(float height) { height_ = height; }

private:
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
    void InitializeCallbacks();

    // -------------------- Destroyers ----------------------
    // Destroys the SurfaceKHR
    void DestroySurface() const;
    static void DestroyOSWindow(OSWindow* p_os_window);

    // -------------------- Callbacks ----------------------

    /**
     * \brief Called when the OS Framebuffer is resized
     * \param width The new framebuffer width
     * \param height The new framebuffer height
     */
    static void OSFramebufferResizeCallback(OSWindow* p_os_window, int width, int height);

    /**
     * \brief Called when the OS Mouse position updates
     * \param x The new x position of the mouse
     * \param y The new y position of the mouse
     */
    static void OSWindowMousePositionCallback(OSWindow* p_os_window, double x, double y);

    /**
     * \brief Called when a key is either pressed, released
     * \param p_os_window The window who we register the callback to
     * \param key The key code
     * \param scancode The key code specific to the OS
     * \param action Press, Release, etc...
     * \param mods Flags that help determine if we are holding SHIFT, CTRL, ALT, etc...
     */
    static void OSWindowKeyCallback(OSWindow* p_os_window, int key, int scancode, int action, int mods);

    /**
     * \brief Called when we get input from a mouse button
     * \param p_os_window The window who we register the callback to
     * \param button The mouse button associated with the input
     * \param action The action from the mouse button
     * \param mods Were we holding SHIT/ALT/CTRL?
     */
    static void OSMouseButtonCallback(OSWindow* p_os_window, int button, int action, int mods);

    // -------------------- Callback Helpers ----------------------
    static Application* GetApplicationFromWindow(OSWindow* p_os_window);

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

    // This window's cursor
    Cursor cursor_;

    // ------------------ Logging --------------------------
    static const std::string kWho;
};

}
}
