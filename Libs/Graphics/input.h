#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace Phyre {
namespace Graphics {

/**
 * \brief Various input mappings that can be used to trigger events.
 * The purpose of this namespaces is to provide a compatible interface
 * to map human input to OS Window related events.
 */
namespace Input {

/**
 * \brief Keyboard key events
 */
enum Key {
    kUnknown = GLFW_KEY_UNKNOWN,
    kSpace = GLFW_KEY_SPACE,
    kApostrophe = GLFW_KEY_APOSTROPHE,
    kComma = GLFW_KEY_COMMA,
    kMinus = GLFW_KEY_MINUS,
    kPeriod = GLFW_KEY_PERIOD,
    kSlash = GLFW_KEY_SLASH,
    k0 = GLFW_KEY_0,
    k1 = GLFW_KEY_1,
    k2 = GLFW_KEY_2,
    k3 = GLFW_KEY_3,
    k4 = GLFW_KEY_4,
    k5 = GLFW_KEY_5,
    k6 = GLFW_KEY_6,
    k7 = GLFW_KEY_7,
    k8 = GLFW_KEY_8,
    k9 = GLFW_KEY_9,
    kSemicolon = GLFW_KEY_SEMICOLON,
    kEqual = GLFW_KEY_EQUAL,
    kA = GLFW_KEY_A,
    kB = GLFW_KEY_B,
    kC = GLFW_KEY_C,
    kD = GLFW_KEY_D,
    kE = GLFW_KEY_E,
    kF = GLFW_KEY_F,
    kG = GLFW_KEY_G,
    kH = GLFW_KEY_H,
    kI = GLFW_KEY_I,
    kJ = GLFW_KEY_J,
    kK = GLFW_KEY_K,
    kL = GLFW_KEY_L,
    kM = GLFW_KEY_M,
    kN = GLFW_KEY_N,
    kO = GLFW_KEY_O,
    kP = GLFW_KEY_P,
    kQ = GLFW_KEY_Q,
    kR = GLFW_KEY_R,
    kS = GLFW_KEY_S,
    kT = GLFW_KEY_T,
    kU = GLFW_KEY_U,
    kV = GLFW_KEY_V,
    kW = GLFW_KEY_W,
    kX = GLFW_KEY_X,
    kY = GLFW_KEY_Y,
    kZ = GLFW_KEY_Z,
    kLeftBracket = GLFW_KEY_LEFT_BRACKET,
    kBackslash = GLFW_KEY_BACKSLASH,
    kRightBracket = GLFW_KEY_RIGHT_BRACKET,
    kBacktick = GLFW_KEY_GRAVE_ACCENT,
    kEscape = GLFW_KEY_ESCAPE,
    kEnter = GLFW_KEY_ENTER,
    kTab = GLFW_KEY_TAB,
    kBackspace = GLFW_KEY_BACKSPACE,
    kInsert = GLFW_KEY_INSERT,
    kDelete = GLFW_KEY_DELETE,
    kRight = GLFW_KEY_RIGHT,
    kLeft = GLFW_KEY_LEFT,
    kDown = GLFW_KEY_DOWN,
    kUp = GLFW_KEY_UP,
    kPageUp = GLFW_KEY_PAGE_UP,
    kPageDown = GLFW_KEY_PAGE_DOWN,
    kHome = GLFW_KEY_HOME,
    kEnd = GLFW_KEY_END,
    kCapsLock = GLFW_KEY_CAPS_LOCK,
    kScrollLock = GLFW_KEY_SCROLL_LOCK,
    kNumLock = GLFW_KEY_NUM_LOCK,
    kPrintScreen = GLFW_KEY_PRINT_SCREEN,
    kPause = GLFW_KEY_PAUSE,
    kF1 = GLFW_KEY_F1,
    kF2 = GLFW_KEY_F2,
    kF3 = GLFW_KEY_F3,
    kF4 = GLFW_KEY_F4,
    kF5 = GLFW_KEY_F5,
    kF6 = GLFW_KEY_F6,
    kF7 = GLFW_KEY_F7,
    kF8 = GLFW_KEY_F8,
    kF9 = GLFW_KEY_F9,
    kF10 = GLFW_KEY_F10,
    kF11 = GLFW_KEY_F11,
    kF12 = GLFW_KEY_F12,
    kF13 = GLFW_KEY_F13,
    kF14 = GLFW_KEY_F14,
    kF15 = GLFW_KEY_F15,
    kF16 = GLFW_KEY_F16,
    kF17 = GLFW_KEY_F17,
    kF18 = GLFW_KEY_F18,
    kF19 = GLFW_KEY_F19,
    kF20 = GLFW_KEY_F20,
    kF21 = GLFW_KEY_F21,
    kF22 = GLFW_KEY_F22,
    kF23 = GLFW_KEY_F23,
    kF24 = GLFW_KEY_F24,
    kF25 = GLFW_KEY_F25,
    kKeyPad0 = GLFW_KEY_KP_0,
    kKeyPad1 = GLFW_KEY_KP_1,
    kKeyPad2 = GLFW_KEY_KP_2,
    kKeyPad3 = GLFW_KEY_KP_3,
    kKeyPad4 = GLFW_KEY_KP_4,
    kKeyPad5 = GLFW_KEY_KP_5,
    kKeyPad6 = GLFW_KEY_KP_6,
    kKeyPad7 = GLFW_KEY_KP_7,
    kKeyPad8 = GLFW_KEY_KP_8,
    kKeyPad9 = GLFW_KEY_KP_9,
    kKeyPadDecimal = GLFW_KEY_KP_DECIMAL,
    kKeyPadDivide = GLFW_KEY_KP_DIVIDE,
    kKeyPadMultiply = GLFW_KEY_KP_MULTIPLY,
    kKeyPadSubtract = GLFW_KEY_KP_SUBTRACT,
    kKeyPadAdd = GLFW_KEY_KP_ADD,
    kKeyPadEnter = GLFW_KEY_KP_ENTER,
    kKeyPadEqual = GLFW_KEY_KP_EQUAL,
    kLeftShift = GLFW_KEY_LEFT_SHIFT,
    kLeftCtrl = GLFW_KEY_LEFT_CONTROL,
    kLeftAlt = GLFW_KEY_LEFT_ALT,
    kLeftSuper = GLFW_KEY_LEFT_SUPER,
    kRightShift = GLFW_KEY_RIGHT_SHIFT,
    kRightCtrl = GLFW_KEY_RIGHT_CONTROL,
    kRightAlt = GLFW_KEY_RIGHT_ALT,
    kRightSuper = GLFW_KEY_RIGHT_SUPER,
    kMenu = GLFW_KEY_MENU
};

std::string KeyToString(Key key);

/**
 * \brief Mouse events
 */
enum Mouse {
    kButton1 = GLFW_MOUSE_BUTTON_1,
    kButton2 = GLFW_MOUSE_BUTTON_2,
    kButton3 = GLFW_MOUSE_BUTTON_3,
    kButton4 = GLFW_MOUSE_BUTTON_4,
    kButton5 = GLFW_MOUSE_BUTTON_5,
    kButton6 = GLFW_MOUSE_BUTTON_6,
    kButton7 = GLFW_MOUSE_BUTTON_7,
    kButton8 = GLFW_MOUSE_BUTTON_8,
    kLeftButton = GLFW_MOUSE_BUTTON_LEFT,
    kRightButton = GLFW_MOUSE_BUTTON_RIGHT,
    kMiddleButton = GLFW_MOUSE_BUTTON_MIDDLE
};

std::string MouseToString(Mouse mouse);

/**
 * \brief An action which is usually associated with an Action
 */
enum Action {
    kPressed = GLFW_PRESS,
    kReleased = GLFW_RELEASE,
    kHold = GLFW_REPEAT
};

std::string ActionToString(Action action);

/**
* \brief A modifier which is generally associated when performing an Action with a given Key
* E.g. Pressing A while holding SHIFT and CTRL
*/
enum Modifier {
    kShift = GLFW_MOD_SHIFT,
    kCtrl = GLFW_MOD_CONTROL,
    kAlt = GLFW_MOD_ALT,
    kSuper = GLFW_MOD_SUPER
};

std::string ModifierToString(Modifier mod);

/**
 * \param modifier_flags Binary flag bits which represent various Modifiers
 * \return A list of modifiers represented as a string
 */
std::string ModifierFlagsToString(int modifier_flags);

}
}
}
