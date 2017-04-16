#include <Logging/logging.h>
#include <vulkan/vulkan.h>

#include "os_window.h"

namespace Phyre {
namespace Input {

const std::string Window::kWho = "[Input::OSWindow]";

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialize GLFW!");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    p_os_window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    DispatchEvents();
}

Window::~Window() {
    glfwDestroyWindow(p_os_window_);
}

vk::SurfaceKHR Window::CreateVulkanSurfaceKHR(const vk::Instance& instance) const {
    VkSurfaceKHR surface;
    VkResult error = glfwCreateWindowSurface(instance, p_os_window_, nullptr, &surface);
    if (error == VK_ERROR_EXTENSION_NOT_PRESENT) {
        throw std::runtime_error("Failed to instantiate vulkan surface: Instance WSI extensions not present");
    }
    if (error != VK_SUCCESS) {
        throw std::runtime_error("Failed to instantiate vulkan surface");
    }
    return surface;
}

void Window::Bind(Handler::Pointer handler) {
    Add(handler);
}

void Window::Unbind(Handler::Pointer handler) {
    Remove(handler);
}

void Window::SetCursorMode(Cursor::Mode mode) const {
    glfwSetInputMode(p_os_window_, GLFW_CURSOR, mode);
}

Cursor::Mode Window::CursorMode() const {
    return static_cast<Cursor::Mode>(glfwGetInputMode(p_os_window_, GLFW_CURSOR));
}

glm::vec2 Window::CursorPosition() const {
    double x, y;
    glfwGetCursorPos(p_os_window_, &x, &y);
    return glm::vec2(x, y);
}

void Window::SetCursorPosition(const glm::vec2& coordinates) const {
    glfwSetCursorPos(p_os_window_, coordinates.x, coordinates.y);
}

bool Window::Update() const {
    glfwPollEvents();
    if (!p_os_window_ || glfwWindowShouldClose(p_os_window_)) {
        return false;
    }
    return true;
}

void Window::DispatchEvents() {
    glfwSetWindowUserPointer(p_os_window_, this);
    glfwSetFramebufferSizeCallback(p_os_window_, &OSFramebufferResizeCallback);
    glfwSetCursorPosCallback(p_os_window_, &OSWindowMousePositionCallback);
    glfwSetKeyCallback(p_os_window_, &OSWindowKeyCallback);
    glfwSetMouseButtonCallback(p_os_window_, &OSMouseButtonCallback);
    glfwSetScrollCallback(p_os_window_, &OSMouseScrollCallback);
}

void Window::Add(Handler::Pointer handler) {
    std::pair<Handler::Priority, Handler::Pointer> pair(handler->priority(), handler);
    handlers_.insert(pair);
}

void Window::Remove(Handler::Pointer handler) {
    handlers_.erase(handler->priority());
}

void Window::OSFramebufferResizeCallback(OSWindow* p_os_window, int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    for (auto pair : p_window->handlers()) {
        Handler::Pointer p_handler = pair.second;
        if (p_handler) {
            p_handler->OnFramebufferResize(width, height);
        } else {
            p_window->Remove(p_handler);
        }
    }
}

void Window::OSWindowMousePositionCallback(OSWindow* p_os_window, double x, double y) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    for (auto pair : p_window->handlers()) {
        Handler::Pointer p_handler = pair.second;
        if (p_handler) {
            p_handler->OnMousePositionUpdate(x, y);
        } else {
            p_window->Remove(p_handler);
        }
    }
}

void Window::OSWindowKeyCallback(OSWindow* p_os_window, int key, int /*scancode*/, int action, int mods) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    for (auto pair : p_window->handlers()) {
        Handler::Pointer p_handler = pair.second;
        if (p_handler) {
            if (action == kPressed) {
                p_handler->OnKeyPress(static_cast<Key>(key), mods);
            }
            else if (action == kReleased) {
                p_handler->OnKeyRelease(static_cast<Key>(key), mods);
            }
            else if (action == kHold) {
                p_handler->OnKeyHold(static_cast<Key>(key), mods);
            }
        } else {
            p_window->Remove(p_handler);
        }
    }
}

void Window::OSMouseButtonCallback(OSWindow* p_os_window, int button, int action, int mods) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    for (auto pair : p_window->handlers()) {
        Handler::Pointer p_handler = pair.second;
        if (p_handler) {
            switch (action) {
            case kPressed:
                p_handler->OnMousePress(static_cast<Mouse>(button), mods);
                break;
            case kReleased:
                p_handler->OnMouseRelease(static_cast<Mouse>(button), mods);
                break;
            default:
                PHYRE_LOG(error, kWho) << "Could not determine mouse action";
            }
        } else {
            p_window->Remove(p_handler);
        }
    }
}

void Window::OSMouseScrollCallback(OSWindow* p_os_window, double x_offset, double y_offset) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    for (auto pair : p_window->handlers()) {
        Handler::Pointer p_handler = pair.second;
        if (p_handler) {
            p_handler->OnMouseScroll(x_offset, y_offset);
        } else {
            p_window->Remove(p_handler);
        }
    }
}

}
}