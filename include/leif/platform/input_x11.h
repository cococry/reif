#pragma once

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

typedef enum {
    LeftMouse = Button1,      // Left mouse button
    RightMouse = Button3,     // Right mouse button
    MiddleMouse = Button2,    // Middle mouse button
    MouseButton4 = Button4,   // Mouse button 4 (scroll up)
    MouseButton5 = Button5,    // Mouse button 5 (scroll down)
    // X11 typically does not have predefined constants for buttons beyond 5
    MouseButton6 = 999,      
    MouseButton7 = 1000,      
    MouseButton8 = 1001 
} lf_mouse_button_t;

typedef enum {
    LF_CURSOR_ARROW        = XC_left_ptr,
    LF_CURSOR_IBEAM        = XC_xterm,
    LF_CURSOR_CROSSHAIR    = XC_crosshair,
    LF_CURSOR_HAND         = XC_hand2,
    LF_CURSOR_HRESIZE      = XC_sb_h_double_arrow,
    LF_CURSOR_VRESIZE      = XC_sb_v_double_arrow,
    LF_CURSOR_COUNT      = 6,
} lf_cursor_type_t;

typedef enum {
    KeyUnknown = 0,

    // Printable keys
    KeySpace = XK_space,
    KeyApostrophe = XK_apostrophe,    // '
    KeyComma = XK_comma,              // ,
    KeyMinus = XK_minus,              // -
    KeyPeriod = XK_period,            // .
    KeySlash = XK_slash,              // /
    Key0 = XK_0,
    Key1 = XK_1,
    Key2 = XK_2,
    Key3 = XK_3,
    Key4 = XK_4,
    Key5 = XK_5,
    Key6 = XK_6,
    Key7 = XK_7,
    Key8 = XK_8,
    Key9 = XK_9,
    KeySemicolon = XK_semicolon,      // ;
    KeyEqual = XK_equal,              // =
    KeyA = XK_A,
    KeyB = XK_B,
    KeyC = XK_C,
    KeyD = XK_D,
    KeyE = XK_E,
    KeyF = XK_F,
    KeyG = XK_G,
    KeyH = XK_H,
    KeyI = XK_I,
    KeyJ = XK_J,
    KeyK = XK_K,
    KeyL = XK_L,
    KeyM = XK_M,
    KeyN = XK_N,
    KeyO = XK_O,
    KeyP = XK_P,
    KeyQ = XK_Q,
    KeyR = XK_R,
    KeyS = XK_S,
    KeyT = XK_T,
    KeyU = XK_U,
    KeyV = XK_V,
    KeyW = XK_W,
    KeyX = XK_X,
    KeyY = XK_Y,
    KeyZ = XK_Z,
    KeyLeftBracket = XK_bracketleft,  // [
    KeyBackslash = XK_backslash,       
    KeyRightBracket = XK_bracketright,// ]
    KeyGraveAccent = XK_grave,       
    KeyWorld1 = 999,  
    KeyWorld2 = 1000,                    

    // Function keys
    KeyEscape = XK_Escape,
    KeyEnter = XK_Return,
    KeyTab = XK_Tab,
    KeyBackspace = XK_BackSpace,
    KeyInsert = XK_Insert,
    KeyDelete = XK_Delete,
    KeyRight = XK_Right,
    KeyLeft = XK_Left,
    KeyDown = XK_Down,
    KeyUp = XK_Up,
    KeyPageUp = XK_Prior,
    KeyPageDown = XK_Next,
    KeyHome = XK_Home,
    KeyEnd = XK_End,
    KeyCapsLock = XK_Caps_Lock,
    KeyScrollLock = XK_Scroll_Lock,
    KeyNumLock = XK_Num_Lock,
    KeyPrintScreen = XK_Print,
    KeyPause = XK_Pause,
    KeyF1 = XK_F1,
    KeyF2 = XK_F2,
    KeyF3 = XK_F3,
    KeyF4 = XK_F4,
    KeyF5 = XK_F5,
    KeyF6 = XK_F6,
    KeyF7 = XK_F7,
    KeyF8 = XK_F8,
    KeyF9 = XK_F9,
    KeyF10 = XK_F10,
    KeyF11 = XK_F11,
    KeyF12 = XK_F12,
    KeyF13 = XK_F13,
    KeyF14 = XK_F14,
    KeyF15 = XK_F15,
    KeyF16 = XK_F16,
    KeyF17 = XK_F17,
    KeyF18 = XK_F18,
    KeyF19 = XK_F19,
    KeyF20 = XK_F20,
    KeyF21 = XK_F21,
    KeyF22 = XK_F22,
    KeyF23 = XK_F23,
    KeyF24 = XK_F24,
    KeyF25 = XK_F25,

    // Keypad
    KeyKp0 = XK_KP_0,
    KeyKp1 = XK_KP_1,
    KeyKp2 = XK_KP_2,
    KeyKp3 = XK_KP_3,
    KeyKp4 = XK_KP_4,
    KeyKp5 = XK_KP_5,
    KeyKp6 = XK_KP_6,
    KeyKp7 = XK_KP_7,
    KeyKp8 = XK_KP_8,
    KeyKp9 = XK_KP_9,
    KeyKpDecimal = XK_KP_Decimal,
    KeyKpDivide = XK_KP_Divide,
    KeyKpMultiply = XK_KP_Multiply,
    KeyKpSubtract = XK_KP_Subtract,
    KeyKpAdd = XK_KP_Add,
    KeyKpEnter = XK_KP_Enter,
    KeyKpEqual = XK_KP_Equal,

    // Modifier keys
    KeyLeftShift = XK_Shift_L,
    KeyLeftControl = XK_Control_L,
    KeyLeftAlt = XK_Alt_L,
    KeyLeftSuper = XK_Super_L,         // Windows key (or Cmd on Mac)
    KeyRightShift = XK_Shift_R,
    KeyRightControl = XK_Control_R,
    KeyRightAlt = XK_Alt_R,
    KeyRightSuper = XK_Super_R,        // Windows key (or Cmd on Mac)
    KeyMenu = XK_Menu

} lf_key_code_t;

