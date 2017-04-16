#pragma once
#include <vulkan.hpp>
#include <boost/utility.hpp>
#include <map>

#include "handler.h"

namespace Phyre {
namespace Input {

class Window : boost::noncopyable {
public:

    // -------------------------- Construction/Destruction -------------------
    /**
    * \param width The width of the window
    * \param height The height of the window
    * \param title The title of the window
    */
    Window(int width, int height, const std::string& title);
    ~Window();
    
    /** 
     * \param instance The vulkan instance required to create the vulkan surface
     * \return A vulkan surface from the OS window
     */
    vk::SurfaceKHR CreateVulkanSurfaceKHR(const vk::Instance& instance) const;

    /**
     * \brief Bind a handler to us
     * \param handler The pointer to the handler we wish to dispatch events to
     */
    void Bind(Handler::Pointer handler);

    /**
     * \brief Unbind a handler from us
     * \param handler The pointer to the handler from which we no longer wish to
     * dispatch events to
     */
    void Unbind(Handler::Pointer handler);

    /**
     * \brief Update the cursor mode for this window
     * \param mode The cursor mode we will be using
     */
    void SetCursorMode(Cursor::Mode mode) const;

    /**
     * \return The cursor mode this window is currently in
     */
    Cursor::Mode CursorMode() const;

    /**
     * \return The position of the cursor in this window
     */
    glm::vec2 CursorPosition() const;
    void SetCursorPosition(const glm::vec2& coordinates) const;

    /**
    * \return Returns true if the window updated successfully.
    */
    bool Update() const;

private:
    // -------------------------- Type Definitions -------------------------
    typedef GLFWwindow OSWindow;
    typedef std::map<Handler::Priority, Handler::Pointer> HandlerMap;

    // ---------------------- Event Dispatching ----------------------------
    
    // Dispatch events to all bound handlers
    void DispatchEvents();

    /**
     * \brief Manage a handler
     * \param handler The handler we want to manage
     */
    void Add(Handler::Pointer handler);

    /**
     * \brief Remove a handler
     * \param handler The handler we wish to stop managing
     */
    void Remove(Handler::Pointer handler);

    // Get the set of handlers we are managing
    const HandlerMap& handlers() const { return handlers_; }

    // ------------------------ OS Window Callbacks ------------------------
    static void OSFramebufferResizeCallback(OSWindow* p_os_window, int width, int height);
    static void OSWindowMousePositionCallback(OSWindow* p_os_window, double x, double y);
    static void OSWindowKeyCallback(OSWindow* p_os_window, int key, int /*scancode*/, int action, int mods);
    static void OSMouseButtonCallback(OSWindow* p_os_window, int button, int action, int mods);
    static void OSMouseScrollCallback(OSWindow* p_os_window, double x_offset, double y_offset);

    // -------------------------- Data Members -----------------------------
    // A pointer to the underlying OS Window
    OSWindow* p_os_window_;

    // The handlers to whom we dispatch events to
    HandlerMap handlers_;

    // -------------------------- Logging Helper ---------------------------
    static const std::string kWho;
};
}
}
