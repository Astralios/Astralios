#pragma once

#include <stdint.h>

typedef enum kbd_key_action_t: uint8_t 
{
   KEY_ACTION_RELEASE,
   KEY_ACTION_PRESS,
} kbd_key_action_t;

typedef enum kbd_key_mode_t: uint8_t 
{
    KEY_MODE_SHIFT = 1 << 0,
    KEY_MODE_CTRL  = 1 << 1,
    KEY_MODE_ALT   = 1 << 2,
    KEY_MODE_CAPS  = 1 << 3,
} kbd_key_mode_t;

typedef struct kbd_event_t 
{
    uint16_t            scancode;
    kbd_key_action_t    action;
    kbd_key_mode_t      mode;
} kbd_event_t;

typedef enum kbd_key_t 
{
    KEY_NONE,
    KEY_ESC,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_MINUS,
    KEY_EQUALS,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_OPEN_BRACKET,
    KEY_CLOSED_BRACKET,
    KEY_ENTER,
    KEY_LEFT_CONTROL,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,
    KEY_SINGLE_QUOTE,
    KEY_BACK_TICK,
    KEY_LEFT_SHIFT,
    KEY_BACKSLASH,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_RIGHT_SHIFT,
    KEY_KEYPAD_ASTERISK,
    KEY_LEFT_ALT,
    KEY_SPACE,
    KEY_CAPSLOCK,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUMBERLOCK,
    KEY_SCROLLLOCK,
    KEY_KEYPAD_7,
    KEY_KEYPAD_8,
    KEY_KEYPAD_9,
    KEY_KEYPAD_MINUS,
    KEY_KEYPAD_4,
    KEY_KEYPAD_5,
    KEY_KEYPAD_6,
    KEY_KEYPAD_PLUS,
    KEY_KEYPAD_1,
    KEY_KEYPAD_2,
    KEY_KEYPAD_3,
    KEY_KEYPAD_0,
    KEY_KEYPAD_PERIOD,
    KEY_F11,
    KEY_F12,
    // TODO: Add special keys
} kbd_key_t;


