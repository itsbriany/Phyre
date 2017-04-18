#include <boost/assign/list_of.hpp>
#include <unordered_map>

#include "input.h"
#include <sstream>


namespace Phyre {
namespace Graphics {

typedef std::unordered_map<Input::Key, std::string> KeyToStringMap;
const KeyToStringMap kKeyToStringMap = boost::assign::map_list_of
    (Input::Key::kUnknown, "UNKNOWN")
    (Input::Key::kSpace, "SPACE")
    (Input::Key::kApostrophe, "'")
    (Input::Key::kComma, ",")
    (Input::Key::kPeriod, ".")
    (Input::Key::kMinus, "-")
    (Input::Key::kSlash, "/")
    (Input::Key::k0, "0")
    (Input::Key::k1, "1")
    (Input::Key::k2, "2")
    (Input::Key::k3, "3")
    (Input::Key::k4, "4")
    (Input::Key::k5, "5")
    (Input::Key::k6, "6")
    (Input::Key::k7, "7")
    (Input::Key::k8, "8")
    (Input::Key::k9, "9")
    (Input::Key::kSemicolon, ";")
    (Input::Key::kEqual, "=")
    (Input::Key::kA, "A")
    (Input::Key::kB, "B")
    (Input::Key::kC, "C")
    (Input::Key::kD, "D")
    (Input::Key::kE, "E")
    (Input::Key::kF, "F")
    (Input::Key::kG, "G")
    (Input::Key::kH, "H")
    (Input::Key::kI, "I")
    (Input::Key::kJ, "J")
    (Input::Key::kK, "K")
    (Input::Key::kL, "L")
    (Input::Key::kM, "M")
    (Input::Key::kN, "N")
    (Input::Key::kO, "O")
    (Input::Key::kP, "P")
    (Input::Key::kQ, "Q")
    (Input::Key::kR, "R")
    (Input::Key::kS, "S")
    (Input::Key::kT, "T")
    (Input::Key::kU, "U")
    (Input::Key::kV, "V")
    (Input::Key::kW, "W")
    (Input::Key::kX, "X")
    (Input::Key::kY, "Y")
    (Input::Key::kZ, "Z")
    (Input::Key::kLeftBracket, "[")
    (Input::Key::kBackslash, "\\")
    (Input::Key::kRightBracket, "]")
    (Input::Key::kBacktick, "`")
    (Input::Key::kEscape, "ESC")
    (Input::Key::kEnter, "ENTER")
    (Input::Key::kTab, "TAB")
    (Input::Key::kBackspace, "BACKSPACE")
    (Input::Key::kInsert, "INSERT")
    (Input::Key::kDelete, "DELETE")
    (Input::Key::kRight, "RIGHT")
    (Input::Key::kDown, "DOWN")
    (Input::Key::kLeft, "LEFT")
    (Input::Key::kUp, "UP")
    (Input::Key::kPageUp, "PAGE_UP")
    (Input::Key::kPageDown, "PAGE_DOWN")
    (Input::Key::kHome, "HOME")
    (Input::Key::kEnd, "END")
    (Input::Key::kCapsLock, "CAPS_LOCK")
    (Input::Key::kScrollLock, "SCROLL_LOCK")
    (Input::Key::kNumLock, "NUM_LOCK")
    (Input::Key::kPrintScreen, "PRINT_SCREEN")
    (Input::Key::kPause, "PAUSE")
    (Input::Key::kF1, "F1")
    (Input::Key::kF2, "F2")
    (Input::Key::kF3, "F3")
    (Input::Key::kF4, "F4")
    (Input::Key::kF5, "F5")
    (Input::Key::kF6, "F6")
    (Input::Key::kF7, "F7")
    (Input::Key::kF8, "F8")
    (Input::Key::kF9, "F9")
    (Input::Key::kF10, "F10")
    (Input::Key::kF11, "F11")
    (Input::Key::kF12, "F12")
    (Input::Key::kF13, "F13")
    (Input::Key::kF14, "F14")
    (Input::Key::kF15, "F15")
    (Input::Key::kF16, "F16")
    (Input::Key::kF17, "F17")
    (Input::Key::kF18, "F18")
    (Input::Key::kF19, "F19")
    (Input::Key::kF20, "F20")
    (Input::Key::kF21, "F21")
    (Input::Key::kF22, "F22")
    (Input::Key::kF23, "F23")
    (Input::Key::kF24, "F24")
    (Input::Key::kF25, "F25")
    (Input::Key::kKeyPad0, "KEY_PAD_0")
    (Input::Key::kKeyPad1, "KEY_PAD_1")
    (Input::Key::kKeyPad2, "KEY_PAD_2")
    (Input::Key::kKeyPad3, "KEY_PAD_3")
    (Input::Key::kKeyPad4, "KEY_PAD_4")
    (Input::Key::kKeyPad5, "KEY_PAD_5")
    (Input::Key::kKeyPad6, "KEY_PAD_6")
    (Input::Key::kKeyPad7, "KEY_PAD_7")
    (Input::Key::kKeyPad8, "KEY_PAD_8")
    (Input::Key::kKeyPad9, "KEY_PAD_9")
    (Input::Key::kKeyPadDecimal, "KEY_PAD_DECIMAL")
    (Input::Key::kKeyPadDivide, "KEY_PAD_DIVIDE")
    (Input::Key::kKeyPadMultiply, "KEY_PAD_MULTIPLY")
    (Input::Key::kKeyPadSubtract, "KEY_PAD_SUBTRACT")
    (Input::Key::kKeyPadAdd, "KEY_PAD_ADD")
    (Input::Key::kKeyPadEnter, "KEY_PAD_ENTER")
    (Input::Key::kKeyPadEqual, "KEY_PAD_EQUAL")
    (Input::Key::kLeftShift, "LEFT_SHIFT")
    (Input::Key::kLeftCtrl, "LEFT_CTRL")
    (Input::Key::kLeftAlt, "LEFT_ALT")
    (Input::Key::kLeftSuper, "LEFT_SUPER")
    (Input::Key::kRightShift, "RIGHT_SHIFT")
    (Input::Key::kRightCtrl, "RIGHT_CTRL")
    (Input::Key::kRightAlt, "RIGHT_ALT")
    (Input::Key::kRightSuper, "RIGHT_SUPER")
    (Input::Key::kMenu, "MENU")
    ;

std::string Input::KeyToString(Key key) {
    KeyToStringMap::const_iterator cit = kKeyToStringMap.find(key);
    if (cit != kKeyToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_KEY";
}

typedef std::unordered_map<Input::Mouse, std::string> MouseToStringMap;
const MouseToStringMap kMouseToStringMap = boost::assign::map_list_of
    (Input::Mouse::kButton1, "BUTTON_1")
    (Input::Mouse::kButton2, "BUTTON_2")
    (Input::Mouse::kButton3, "BUTTON_3")
    (Input::Mouse::kButton4, "BUTTON_4")
    (Input::Mouse::kButton5, "BUTTON_5")
    (Input::Mouse::kButton6, "BUTTON_6")
    (Input::Mouse::kButton7, "BUTTON_7")
    (Input::Mouse::kButton8, "BUTTON_8")
    (Input::Mouse::kLeftButton, "LEFT_BUTTON")
    (Input::Mouse::kRightButton, "RIGHT_BUTTON")
    (Input::Mouse::kMiddleButton, "MIDDLE_BUTTON")
    ;

std::string Input::MouseToString(Mouse mouse_button) {
    MouseToStringMap::const_iterator cit = kMouseToStringMap.find(mouse_button);
    if (cit != kMouseToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_MOUSE_BUTTON";
}

typedef std::unordered_map<Input::Action, std::string> ActionToStringMap;
const ActionToStringMap kActionToStringMap = boost::assign::map_list_of
    (Input::Action::kHold, "HOLD")
    (Input::Action::kPressed, "PRESSED")
    (Input::Action::kReleased, "RELEASED")
    ;

std::string Input::ActionToString(Action action) {
    ActionToStringMap::const_iterator cit = kActionToStringMap.find(action);
    if (cit != kActionToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_ACTION";
}

typedef std::unordered_map<Input::Modifier, std::string> ModifierToStringMap;
const ModifierToStringMap kModifierToStringMap = boost::assign::map_list_of
    (Input::Modifier::kAlt, "ALT")
    (Input::Modifier::kCtrl, "CTRL")
    (Input::Modifier::kShift, "SHIFT")
    (Input::Modifier::kSuper, "SUPER")
    ;

std::string Input::ModifierToString(Modifier mod) {
    ModifierToStringMap::const_iterator cit = kModifierToStringMap.find(mod);
    if (cit != kModifierToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_MODIFIER";
}

std::string Input::ModifierFlagsToString(int modifier_flags) {
    std::vector<std::string> mod_names;
    if (modifier_flags & Modifier::kShift) {
        mod_names.emplace_back("SHIFT");
    }
    if (modifier_flags & Modifier::kCtrl) {
        mod_names.emplace_back("CTRL");
    }
    if (modifier_flags & Modifier::kAlt) {
        mod_names.emplace_back("ALT");
    }
    if (modifier_flags & Modifier::kSuper) {
        mod_names.emplace_back("SUPER");
    }
    std::ostringstream oss;
    oss << '[';
    if (!mod_names.empty()) {
        for (int i = 0; i < mod_names.size(); ++i) {
            if (i + 1 >= mod_names.size()) {
                break;
            }
            oss << mod_names[i] << ", ";
        }
        oss << mod_names.back();
    }
    oss << ']';
    return oss.str();
}

typedef std::unordered_map<Input::CursorMode, std::string> CursorModeToStringMap;
const CursorModeToStringMap kCursorModeToStringMap = boost::assign::map_list_of
    (Input::CursorMode::kDisabled, "DISABLED")
    (Input::CursorMode::kNormal, "NORMAL")
    (Input::CursorMode::kHidden, "HIDDEN")
    ;

std::string Input::CursorModeToString(CursorMode cursor_mode) {
    CursorModeToStringMap::const_iterator cit = kCursorModeToStringMap.find(cursor_mode);
    if (cit != kCursorModeToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_CURSOR_MODE";
}
}
}
