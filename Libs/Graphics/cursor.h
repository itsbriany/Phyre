#pragma once
#include "input.h"

namespace Phyre {
namespace Graphics {

class Cursor {
public:
    //------------------- Construction/Destruction --------------------
    Cursor(OSWindow* p_window);
    ~Cursor();

    //------------------------- Accessors ------------------------------
    Input::CursorMode mode() const { return mode_; }
    void set_mode(Input::CursorMode mode);

private:
    //------------------------ Data Members ----------------------------
    // The OS Window
    OSWindow *p_window_;

    // The mode of the cursor
    Input::CursorMode mode_;

    //------------------------ Logging Helper --------------------------
    static const std::string kWho;
};

}
}
