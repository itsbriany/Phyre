#pragma once
#include <glm/detail/type_vec2.hpp>
#include <memory>

#include "input.h"

namespace Phyre {
namespace Input {
class Window;

/**
 * \brief Inherit this to handle input in your application
 */
class Handler : public std::enable_shared_from_this<Handler> {
public:
    //---------------------- Type Definitions ------------------------
    typedef std::shared_ptr<Handler> Pointer;
    typedef std::weak_ptr<Handler> Weak;
    
    // The lower this number, the higher the priority.
    // This means that a lower number will get events dispatched to it before 
    // a handler with a higher priority number
    typedef uint32_t Priority;

    //---------------------- Construction/Destruction --------------------
    Handler(Priority priority);
    virtual ~Handler();

    //------------------------ Interface -----------------------------

    Priority priority() const { return priority_; }

    /**
     * \brief Bind this object to the OS Window so that we can handle
     * events from it.
     * NOTE: The object must be fully allocated within a shared_ptr before
     * you can bind to anything.
     * \param p_window A pointer to the OS Window we wish to bind to.
     */
    void Bind(Window* p_window);

    /**
     * \return The current cursor mode. This can be useful to know whether the
     * cursor is visible or not. We may also wish to make the cursor move in an
     * infinite world.
     */
    Cursor::Mode CursorMode() const;
    void SetCursorMode(Cursor::Mode mode) const;

    /**
     * \return The cursor's position as a 2D point in the current world.
     */
    glm::vec2 CursorPosition() const;
    void SetCursorPosition(const glm::vec2& coordinates) const;

    /**
     * \param mouse_button The mouse button we wish to query state from
     * \return The state of the mouse button (i.e. pressed or released)
     */
    Action MouseButton(Mouse mouse_button) const;

    //---------------------- Window Handles --------------------------

    /**
    * \brief Called when the window's framebuffer gets resized
    * \param width the width in screen resolution pixels
    * \param height the height in screen resolution pixels
    */
    virtual void OnFramebufferResize(int width, int height);

    /**
    * \brief Called when the mouse changes position
    * \param x The new x position of the mouse
    * \param y The new y position of the mouse
    */
    virtual void OnMousePositionUpdate(double x, double y);

    /**
    * \brief Called when the key with the given code is pressed
    * \param key Which key did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnKeyPress(Key key, int mods);

    /**
    * \brief Called when the key with the given code is pressed
    * \param key Which key did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnKeyRelease(Key key, int mods);

    /**
    * \brief Called when the key with the given code is pressed
    * \param key Which key did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnKeyHold(Key key, int mods);

    /**
    * \brief Called when a mouse button was pressed
    * \param mouse_button  Which mouse button did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnMousePress(Mouse mouse_button, int mods);

    /**
    * \brief Called when a mouse button was released
    * \param mouse_button  Which mouse button did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnMouseRelease(Mouse mouse_button, int mods);

    /**
    * \brief Called when the mouse wheel is scrolled
    * \param x_offset How fast we are scolling on the x axis
    * \param y_offset How fast we are scolling on the y axis
    */
    virtual void OnMouseScroll(double x_offset, double y_offset);

private:
    // ---------------------- Data Members ---------------------------
    // A pointer to the Input Window we are bound to
    Window* p_window_;

    // Our priority for getting events
    Priority priority_;

    //---------------------- Logging Helper --------------------------
    static const std::string kWho;
};

}
}