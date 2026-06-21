#include "libs/libinput/kbd.h"
#include <drvs/ps2/kbd/scancodes.h>
#include <misc/todo.h>
#include <stdbool.h>

static key_t scancode_set_1[] = {
    KEY_NONE, KEY_ESC, KEY_1, KEY_2, KEY_3,
    KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_MINUS, KEY_EQUALS, KEY_BACKSPACE, KEY_TAB,
    KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I,
    KEY_O, KEY_P, KEY_OPEN_BRACKET, KEY_CLOSED_BRACKET, KEY_ENTER,
    KEY_LEFT_CONTROL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H,
    KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_SINGLE_QUOTE, KEY_BACK_TICK,
    KEY_LEFT_SHIFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
    KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_SHIFT,
    KEY_KEYPAD_ASTERISK, KEY_LEFT_ALT, KEY_SPACE, KEY_CAPSLOCK, KEY_F1,
    KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,
    KEY_F10, KEY_NUMBERLOCK, KEY_SCROLLLOCK, KEY_KEYPAD_7, KEY_KEYPAD_8,
    KEY_KEYPAD_9, KEY_KEYPAD_MINUS, KEY_KEYPAD_4, KEY_KEYPAD_5,
    KEY_KEYPAD_6, KEY_KEYPAD_PLUS, KEY_KEYPAD_1, KEY_KEYPAD_2,
    KEY_KEYPAD_3, KEY_KEYPAD_0, KEY_KEYPAD_PERIOD,
    KEY_NONE, KEY_NONE, KEY_NONE, KEY_F11, KEY_F12
};

static key_t scancode_set_2[] = {
    KEY_NONE,  KEY_F9, KEY_NONE, KEY_F5,
    KEY_F4, KEY_F1, KEY_F2, KEY_F12,
    KEY_NONE, KEY_F10, KEY_F8, KEY_F6,
    KEY_F4, KEY_TAB, KEY_BACK_TICK, KEY_NONE,
    KEY_NONE, KEY_LEFT_ALT, KEY_LEFT_SHIFT, KEY_NONE,
    KEY_LEFT_CONTROL, KEY_Q, KEY_1, KEY_NONE,
    KEY_NONE, KEY_NONE, KEY_Z, KEY_S, KEY_A,
    KEY_W, KEY_2, KEY_NONE, KEY_NONE, KEY_C, KEY_X,
    KEY_D, KEY_E, KEY_4, KEY_3, KEY_NONE, KEY_NONE, KEY_SPACE,
    KEY_V, KEY_F, KEY_T, KEY_R, KEY_5, KEY_NONE, KEY_NONE,
    KEY_N, KEY_B, KEY_H, KEY_G, KEY_Y, KEY_6, KEY_NONE, KEY_NONE,
    KEY_NONE, KEY_M, KEY_J, KEY_U, KEY_7, KEY_8, KEY_NONE, KEY_NONE,
    KEY_COMMA, KEY_K, KEY_I, KEY_O, KEY_0, KEY_9, KEY_NONE, KEY_NONE,
    KEY_PERIOD, KEY_SLASH, KEY_L, KEY_SEMICOLON, KEY_P, KEY_MINUS, KEY_NONE,
    KEY_NONE, KEY_NONE, KEY_SINGLE_QUOTE, KEY_NONE, KEY_OPEN_BRACKET,
    KEY_EQUALS, KEY_NONE, KEY_NONE, KEY_CAPSLOCK, KEY_RIGHT_SHIFT, KEY_ENTER,
    KEY_CLOSED_BRACKET, KEY_NONE, KEY_BACKSLASH, KEY_NONE, KEY_NONE,
    KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE,
    KEY_NONE, KEY_NONE, KEY_BACKSPACE, KEY_NONE,
    KEY_NONE, KEY_KEYPAD_1, KEY_NONE, KEY_KEYPAD_4,
    KEY_KEYPAD_7, KEY_NONE, KEY_NONE, KEY_NONE,
    KEY_KEYPAD_0, KEY_KEYPAD_PERIOD, KEY_KEYPAD_2, KEY_KEYPAD_5,
    KEY_KEYPAD_6, KEY_KEYPAD_8, KEY_ESC, KEY_NUMBERLOCK,
    KEY_F11, KEY_KEYPAD_PLUS, KEY_KEYPAD_3, KEY_KEYPAD_MINUS,
    KEY_KEYPAD_ASTERISK, KEY_KEYPAD_8, KEY_SCROLLLOCK, KEY_NONE,
    KEY_NONE, KEY_NONE, KEY_NONE, KEY_F7,
};

static key_t scancode_set_2_extended[] = {
    [0x10] = KEY_WWW_SEARCH,
    KEY_RIGHT_ALT,
    [0x14] = KEY_RIGHT_CONTROL,
    KEY_PREVIOUS_TRACK,
    [0x18] = KEY_WWW_FAVORITES,
    [0x1F] = KEY_LEFT_GUI,
    KEY_WWW_REFRESH,
    KEY_VOLUME_DOWN,
    [0x23] = KEY_MUTE,
    [0x27] = KEY_RIGHT_GUI,
    KEY_WWW_STOP,
    [0x2B] = KEY_CALCULATOR,
    [0x2F] = KEY_APPS,
    KEY_WWW_FORWARD,
    [0x32] = KEY_VOLUME_UP,
    [0x34] = KEY_PLAY,
    [0x37] = KEY_ACPI_POWER,
    KEY_WWW_BACK,
    [0x3A] = KEY_WWW_HOME,
    KEY_STOP,
    [0x3F] = KEY_ACPI_SLEEP,
    KEY_MY_COMPUTER,
    [0x48] = KEY_EMAIL,
    [0x4A] = KEY_KEYPAD_SLASH,
    [0x4D] = KEY_NEXT_TRACK,
    [0x50] = KEY_MEDIA_SELECT,
    [0x5A] = KEY_KEYPAD_ENTER,
    [0x5E] = KEY_ACPI_WAKE,
    [0x69] = KEY_END,
    [0x6B] = KEY_CURSOR_LEFT,
    KEY_HOME,
    [0x70] = KEY_INSERT,
    KEY_DELETE,
    KEY_CURSOR_DOWN,
    [0x74] = KEY_CURSOR_RIGHT,
    KEY_CURSOR_UP,
    [0x7A] = KEY_PAGE_DOWN,
    [0x7D] = KEY_PAGE_UP
};

// TODO: Scancode set 3 support

kbd_ev_t ps2_decodeFromScancode(uint8_t sc, uint8_t set)
{
    static bool released = false;

    kbd_ev_t ev = {0};

    switch (set) {
    case 1:
        todo(ps2_decodeFromScancode, "Unimplemented");
        break;
    case 2:
    {
        static bool extended = false;

        if (sc == 0xE0)
        {
            extended = true;
            goto end;
        }

        if (sc == 0xF0)
        {
            released = true;
            goto end;
        }

        
        if (extended)
        {
            ev.keycode = scancode_set_2_extended[sc];
        }
        else 
        {
            ev.keycode = scancode_set_2[sc];
        }

        ev.action = released ? KEY_ACTION_RELEASE : KEY_ACTION_PRESS;

        released = false;
        extended = false;
        break;
    }
    case 3:
        todo(ps2_decodeFromScancode, "Unimplemented!");
        break;
    default:
        goto end;
    }

end:
    return ev;   
}
