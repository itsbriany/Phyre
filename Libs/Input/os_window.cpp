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
    p_handlers_ = std::make_shared<HandlerMap>();
    InitializeMouseActionMap();
    DispatchEvents();
}

Window::~Window() {
    Close();
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

void Window::Close() {
    if (p_os_window_) {
        glfwDestroyWindow(p_os_window_);
        p_os_window_ = nullptr;
    }
}

void Window::Bind(Handler::Pointer handler) const {
    Add(handler);
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

Action Window::MouseButton(Mouse mouse_button) const {
    MouseActionMap::const_iterator cit = mouse_action_map_.find(mouse_button);
    if (cit != mouse_action_map_.end()) {
        return cit->second;
    }
    return kReleased;
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
    glfwSetWindowCloseCallback(p_os_window_, &OSWindowCloseCallback);
}

void Window::Add(Handler::Pointer handler) const {
    std::pair<Handler::Priority, Handler::Pointer> pair(handler->priority(), handler);
    p_handlers_->insert(pair);
}

void Window::InitializeMouseActionMap() {
    mouse_action_map_ = {
        { kButton1, kReleased },
        { kButton2, kReleased },
        { kButton3, kReleased },
        { kButton4, kReleased },
        { kButton5, kReleased },
        { kButton6, kReleased },
        { kButton7, kReleased },
        { kButton8, kReleased },
        { kLeftButton, kReleased },
        { kMiddleButton, kReleased },
        { kRightButton, kReleased }
    };
}

void Window::OSFramebufferResizeCallback(OSWindow* p_os_window, int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    NotifyHandlers(p_os_window, [width, height](Handler::Pointer p_handler) {
        p_handler->OnFramebufferResize(width, height);
    });
}

void Window::OSWindowMousePositionCallback(OSWindow* p_os_window, double x, double y) {
    NotifyHandlers(p_os_window, [x, y](Handler::Pointer p_handler) {
        p_handler->OnMousePositionUpdate(x, y);
    });
}

void Window::OSWindowKeyCallback(OSWindow* p_os_window, int key, int /*scancode*/, int action, int mods) {
    NotifyHandlers(p_os_window, [key, action, mods](Handler::Pointer p_handler) {
        if (action == kPressed) {
            p_handler->OnKeyPress(static_cast<Key>(key), mods);
        }
        else if (action == kReleased) {
            p_handler->OnKeyRelease(static_cast<Key>(key), mods);
        }
        else if (action == kHold) {
            p_handler->OnKeyHold(static_cast<Key>(key), mods);
        }
    });
}

void Window::OSMouseButtonCallback(OSWindow* p_os_window, int button, int action, int mods) {
    UpdateMouseButton(p_os_window, button, action);
    NotifyHandlers(p_os_window, [button, action, mods](Handler::Pointer p_handler) {
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
    });
}

void Window::OSMouseScrollCallback(OSWindow* p_os_window, double x_offset, double y_offset) {
    NotifyHandlers(p_os_window, [x_offset, y_offset](Handler::Pointer p_handler) {
        p_handler->OnMouseScroll(x_offset, y_offset);
    });
}

void Window::OSWindowCloseCallback(OSWindow* p_os_window) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    p_window->Close();
}

void Window::NotifyHandlers(OSWindow* p_os_window, NotificationCallback callback) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    std::shared_ptr<HandlerMap> p_handlers = p_window->handlers();
    HandlerMap::iterator it = p_handlers->begin();
    while (it != p_handlers->end()) {
        Handler::Weak p_weak_handler = it->second;
        if (Handler::Pointer p_handler = p_weak_handler.lock()) {
            if (p_handler) {
                callback(p_handler);
                ++it;
            } else {
                it = p_handlers->erase(it);
            }
        } else {
            it = p_handlers->erase(it);
        }
    }
}

void Window::UpdateMouseButton(OSWindow* p_os_window, int button, int action) {
    Window *p_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(p_os_window));
    Mouse mouse_button = static_cast<Mouse>(button);
    Action mouse_action = static_cast<Action>(action);
    p_window->mouse_action_map_[mouse_button] = mouse_action;
}

}
}