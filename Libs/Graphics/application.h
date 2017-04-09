#pragma once

#include "input.h"

namespace Phyre {
namespace Graphics {
        
/**
 * \brief The base class from which all Vulkan applications should inherit from
 */
class Application {
public:
    //---------------------- Construction/Destruction ----------------
    Application();
    virtual ~Application();

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
    virtual void OnKeyPress(Input::Key key, int mods);

    /**
    * \brief Called when the key with the given code is pressed
    * \param key Which key did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnKeyRelease(Input::Key key, int mods);

    /**
    * \brief Called when the key with the given code is pressed
    * \param key Which key did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnKeyHold(Input::Key key, int mods);

    /**
     * \brief Called when a mouse button was pressed
     * \param mouse_button  Which mouse button did we press?
     * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
     */
    virtual void OnMousePress(Input::Mouse mouse_button, int mods);

    /**
    * \brief Called when a mouse button was released
    * \param mouse_button  Which mouse button did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnMouseRelease(Input::Mouse mouse_button, int mods);

    /**
    * \brief Called when a mouse button was held
    * \param mouse_button  Which mouse button did we press?
    * \param mods Did we hold down keys like SHIFT/ALT/CTRL?
    */
    virtual void OnMouseHold(Input::Mouse mouse_button, int mods);

private:
    //---------------------- Logging Helper --------------------------
    static const std::string kWho;
};

}
}
