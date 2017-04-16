#include <set>
#include <Logging/logging.h>

#include "os_window.h"
#include "handler.h"
#include "input.h"

namespace Phyre {
namespace Input {

const std::string Handler::kWho = "[Input::Handler]";

Handler::Handler(Priority priority) : 
    p_window_(nullptr),
    priority_(priority)
{
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

Handler::~Handler() {
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

void Handler::Bind(Window* p_window) {
    p_window_ = p_window;
    p_window_->Bind(shared_from_this());
}

void Handler::SetCursorMode(Cursor::Mode mode) const {
    if (!p_window_) {
        PHYRE_LOG(error, kWho) << "Failed to set cursor mode: We are not bound to any window";
        return;
    }
    p_window_->SetCursorMode(mode);
}

Cursor::Mode Handler::CursorMode() const {
    if (!p_window_) {
        PHYRE_LOG(error, kWho) << "Failed to get cursor mode: We are not bound to any window";
        return Cursor::Mode::kNormal;
    }
    return p_window_->CursorMode();
}

glm::vec2 Handler::CursorPosition() const {
    if (!p_window_) {
        PHYRE_LOG(error, kWho) << "Failed to get cursor position: We are not bound to any window";
        return glm::vec2(0, 0);
    }
    return p_window_->CursorPosition();
}

void Handler::SetCursorPosition(const glm::vec2& coordinates) const {
    if (!p_window_) {
        PHYRE_LOG(error, kWho) << "Failed to set cursor position: We are not bound to any window";
        return;
    }
    p_window_->SetCursorPosition(coordinates);
}

void Handler::OnFramebufferResize(int width, int height) {
    PHYRE_LOG(trace, kWho) << "Framebuffer resized: (" << width << 'x' << height << ')';
}

void Handler::OnMousePositionUpdate(double x, double y) {
    PHYRE_LOG(trace, kWho) << "Mouse position updated: (" << x << ", " << y << ')';
}

void Handler::OnKeyPress(Key key, int mods) {
    PHYRE_LOG(trace, kWho) << "Key " << ToString(key) << " was pressed with the following mods: " << FlagsToString(mods);
}

void Handler::OnKeyRelease(Key key, int mods) {
    PHYRE_LOG(trace, kWho) << "Key " << ToString(key) << " was released with the following mods: " << FlagsToString(mods);
}

void Handler::OnKeyHold(Key key, int mods) {
    PHYRE_LOG(trace, kWho) << "Key " << ToString(key) << " was held with the following mods: " << FlagsToString(mods);
}

void Handler::OnMousePress(Mouse mouse_button, int mods) {
    PHYRE_LOG(trace, kWho) << "Mouse " << ToString(mouse_button) << " was pressed with the following mods: " << FlagsToString(mods);
}

void Handler::OnMouseRelease(Mouse mouse_button, int mods) {
    PHYRE_LOG(trace, kWho) << "Mouse " << ToString(mouse_button) << " was released with the following mods: " << FlagsToString(mods);
}

void Handler::OnMouseScroll(double x_offset, double y_offset) {
    PHYRE_LOG(trace, kWho) << "Scroll position: (" << x_offset << ", " << y_offset << ')';
}

}
}
