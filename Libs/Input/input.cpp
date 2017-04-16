#include <boost/assign/list_of.hpp>
#include <unordered_map>
#include <sstream>

#include "input.h"

namespace Phyre {
namespace Input {
namespace Cursor {

const std::string kWho = "[Graphics::Cursor]";

typedef std::unordered_map<enum Mode, std::string> CursorModeToStringMap;
const CursorModeToStringMap kCursorModeToStringMap = boost::assign::map_list_of
    (kDisabled, "DISABLED")
    (kNormal, "NORMAL")
    (kHidden, "HIDDEN")
    ;

std::string ModeToString(enum Mode cursor_mode) {
    CursorModeToStringMap::const_iterator cit = kCursorModeToStringMap.find(cursor_mode);
    if (cit != kCursorModeToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_CURSOR_MODE";
}
}

typedef std::unordered_map<Mouse, std::string> MouseToStringMap;
const MouseToStringMap kMouseToStringMap = boost::assign::map_list_of
    (kButton1, "BUTTON_1")
    (kButton2, "BUTTON_2")
    (kButton3, "BUTTON_3")
    (kButton4, "BUTTON_4")
    (kButton5, "BUTTON_5")
    (kButton6, "BUTTON_6")
    (kButton7, "BUTTON_7")
    (kButton8, "BUTTON_8")
    (kLeftButton, "LEFT_BUTTON")
    (kRightButton, "RIGHT_BUTTON")
    (kMiddleButton, "MIDDLE_BUTTON")
    ;

std::string ToString(Mouse mouse_button) {
    MouseToStringMap::const_iterator cit = kMouseToStringMap.find(mouse_button);
    if (cit != kMouseToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_MOUSE_BUTTON";
}

typedef std::unordered_map<Key, std::string> KeyToStringMap;
const KeyToStringMap kKeyToStringMap = boost::assign::map_list_of
    (kUnknown, "UNKNOWN")
    (kSpace, "SPACE")
    (kApostrophe, "'")
    (kComma, ",")
    (kPeriod, ".")
    (kMinus, "-")
    (kSlash, "/")
    (k0, "0")
    (k1, "1")
    (k2, "2")
    (k3, "3")
    (k4, "4")
    (k5, "5")
    (k6, "6")
    (k7, "7")
    (k8, "8")
    (k9, "9")
    (kSemicolon, ";")
    (kEqual, "=")
    (kA, "A")
    (kB, "B")
    (kC, "C")
    (kD, "D")
    (kE, "E")
    (kF, "F")
    (kG, "G")
    (kH, "H")
    (kI, "I")
    (kJ, "J")
    (kK, "K")
    (kL, "L")
    (kM, "M")
    (kN, "N")
    (kO, "O")
    (kP, "P")
    (kQ, "Q")
    (kR, "R")
    (kS, "S")
    (kT, "T")
    (kU, "U")
    (kV, "V")
    (kW, "W")
    (kX, "X")
    (kY, "Y")
    (kZ, "Z")
    (kLeftBracket, "[")
    (kBackslash, "\\")
    (kRightBracket, "]")
    (kBacktick, "`")
    (kEscape, "ESC")
    (kEnter, "ENTER")
    (kTab, "TAB")
    (kBackspace, "BACKSPACE")
    (kInsert, "INSERT")
    (kDelete, "DELETE")
    (kRight, "RIGHT")
    (kDown, "DOWN")
    (kLeft, "LEFT")
    (kUp, "UP")
    (kPageUp, "PAGE_UP")
    (kPageDown, "PAGE_DOWN")
    (kHome, "HOME")
    (kEnd, "END")
    (kCapsLock, "CAPS_LOCK")
    (kScrollLock, "SCROLL_LOCK")
    (kNumLock, "NUM_LOCK")
    (kPrintScreen, "PRINT_SCREEN")
    (kPause, "PAUSE")
    (kF1, "F1")
    (kF2, "F2")
    (kF3, "F3")
    (kF4, "F4")
    (kF5, "F5")
    (kF6, "F6")
    (kF7, "F7")
    (kF8, "F8")
    (kF9, "F9")
    (kF10, "F10")
    (kF11, "F11")
    (kF12, "F12")
    (kF13, "F13")
    (kF14, "F14")
    (kF15, "F15")
    (kF16, "F16")
    (kF17, "F17")
    (kF18, "F18")
    (kF19, "F19")
    (kF20, "F20")
    (kF21, "F21")
    (kF22, "F22")
    (kF23, "F23")
    (kF24, "F24")
    (kF25, "F25")
    (kKeyPad0, "KEY_PAD_0")
    (kKeyPad1, "KEY_PAD_1")
    (kKeyPad2, "KEY_PAD_2")
    (kKeyPad3, "KEY_PAD_3")
    (kKeyPad4, "KEY_PAD_4")
    (kKeyPad5, "KEY_PAD_5")
    (kKeyPad6, "KEY_PAD_6")
    (kKeyPad7, "KEY_PAD_7")
    (kKeyPad8, "KEY_PAD_8")
    (kKeyPad9, "KEY_PAD_9")
    (kKeyPadDecimal, "KEY_PAD_DECIMAL")
    (kKeyPadDivide, "KEY_PAD_DIVIDE")
    (kKeyPadMultiply, "KEY_PAD_MULTIPLY")
    (kKeyPadSubtract, "KEY_PAD_SUBTRACT")
    (kKeyPadAdd, "KEY_PAD_ADD")
    (kKeyPadEnter, "KEY_PAD_ENTER")
    (kKeyPadEqual, "KEY_PAD_EQUAL")
    (kLeftShift, "LEFT_SHIFT")
    (kLeftCtrl, "LEFT_CTRL")
    (kLeftAlt, "LEFT_ALT")
    (kLeftSuper, "LEFT_SUPER")
    (kRightShift, "RIGHT_SHIFT")
    (kRightCtrl, "RIGHT_CTRL")
    (kRightAlt, "RIGHT_ALT")
    (kRightSuper, "RIGHT_SUPER")
    (kMenu, "MENU")
    ;

std::string ToString(Key key) {
    KeyToStringMap::const_iterator cit = kKeyToStringMap.find(key);
    if (cit != kKeyToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_KEY";
}

typedef std::unordered_map<Action, std::string> ActionToStringMap;
const ActionToStringMap kActionToStringMap = boost::assign::map_list_of
    (kHold, "HOLD")
    (kPressed, "PRESSED")
    (kReleased, "RELEASED")
    ;

std::string ActionToString(Action action) {
    ActionToStringMap::const_iterator cit = kActionToStringMap.find(action);
    if (cit != kActionToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_ACTION";
}

typedef std::unordered_map<Modifier, std::string> ModifierToStringMap;
const ModifierToStringMap kModifierToStringMap = boost::assign::map_list_of
    (kAlt, "ALT")
    (kCtrl, "CTRL")
    (kShift, "SHIFT")
    (kSuper, "SUPER")
    ;

std::string ToString(Modifier mod) {
    ModifierToStringMap::const_iterator cit = kModifierToStringMap.find(mod);
    if (cit != kModifierToStringMap.end()) {
        return cit->second;
    }
    return "UNKNOWN_MODIFIER";
}

std::string FlagsToString(int modifier_flags) {
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

}
}
