#include "cursor.h"
#include "Logging/logging.h"

namespace Phyre {
namespace Graphics {

const std::string Cursor::kWho = "[Graphics::Cursor]";

Cursor::Cursor(OSWindow* p_window) : 
    p_window_(p_window),
    mode_(Input::CursorMode::kNormal)
{
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

Cursor::~Cursor() {
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

void Cursor::set_mode(Input::CursorMode mode) {
    mode_ = mode;
    glfwSetInputMode(p_window_, GLFW_CURSOR, mode);
}

}
}
