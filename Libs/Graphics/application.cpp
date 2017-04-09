#include <vector>
#include <Logging/logging.h>

#include "application.h"

namespace Phyre {
namespace Graphics {

const std::string Application::kWho = "[Graphics::Application]";

Application::Application() {
    PHYRE_LOG(trace, kWho) << "Instantiated";
}

Application::~Application() {
    PHYRE_LOG(trace, kWho) << "Destroyed";
}

void Application::OnFramebufferResize(int width, int height) {
    PHYRE_LOG(trace, kWho) << "Framebuffer resized: (" << width << 'x' << height << ')';
}

void Application::OnMousePositionUpdate(double x, double y) {
    PHYRE_LOG(trace, kWho) << "Mouse position updated: (" << x << ", " << y << ')';
}

void Application::OnKeyPress(Input::Key key, int mods) {
    PHYRE_LOG(trace, kWho) << "Key " << Input::KeyToString(key) << " was pressed with the following mods: " << Input::ModifierFlagsToString(mods);
}

void Application::OnKeyRelease(Input::Key key, int mods) {
    PHYRE_LOG(trace, kWho) << "Key " << Input::KeyToString(key) << " was released with the following mods: " << Input::ModifierFlagsToString(mods);
}

void Application::OnKeyHold(Input::Key key, int mods) {
    PHYRE_LOG(trace, kWho) << "Key " << Input::KeyToString(key) << " was held with the following mods: " << Input::ModifierFlagsToString(mods);
}

void Application::OnMousePress(Input::Mouse mouse_button, int mods) {
    PHYRE_LOG(trace, kWho) << "Mouse " << Input::MouseToString(mouse_button) << " was pressed with the following mods: " << Input::ModifierFlagsToString(mods);
}

void Application::OnMouseRelease(Input::Mouse mouse_button, int mods) {
    PHYRE_LOG(trace, kWho) << "Mouse " << Input::MouseToString(mouse_button) << " was released with the following mods: " << Input::ModifierFlagsToString(mods);
}

}
}
