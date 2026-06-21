#include <misc/helpers.h>
#include <drvs/ps2/kbd/misc.h>

const char *key_to_string(key_t key) {
    switch (key) {
        case KEY_NONE: return stringify(KEY_NONE);
        case KEY_ESC: return stringify(KEY_ESC);
        case KEY_1: return stringify(KEY_1);
        case KEY_2: return stringify(KEY_2);
        case KEY_3: return stringify(KEY_3);
        case KEY_4: return stringify(KEY_4);
        case KEY_5: return stringify(KEY_5);
        case KEY_6: return stringify(KEY_6);
        case KEY_7: return stringify(KEY_7);
        case KEY_8: return stringify(KEY_8);
        case KEY_9: return stringify(KEY_9);
        case KEY_0: return stringify(KEY_0);
        case KEY_MINUS: return stringify(KEY_MINUS);
        case KEY_EQUALS: return stringify(KEY_EQUALS);
        case KEY_BACKSPACE: return stringify(KEY_BACKSPACE);
        case KEY_TAB: return stringify(KEY_TAB);
        case KEY_Q: return stringify(KEY_Q);
        case KEY_W: return stringify(KEY_W);
        case KEY_E: return stringify(KEY_E);
        case KEY_R: return stringify(KEY_R);
        case KEY_T: return stringify(KEY_T);
        case KEY_Y: return stringify(KEY_Y);
        case KEY_U: return stringify(KEY_U);
        case KEY_I: return stringify(KEY_I);
        case KEY_O: return stringify(KEY_O);
        case KEY_P: return stringify(KEY_P);
        case KEY_OPEN_BRACKET: return stringify(KEY_OPEN_BRACKET);
        case KEY_CLOSED_BRACKET: return stringify(KEY_CLOSED_BRACKET);
        case KEY_ENTER: return stringify(KEY_ENTER);
        case KEY_LEFT_CONTROL: return stringify(KEY_LEFT_CONTROL);
        case KEY_A: return stringify(KEY_A);
        case KEY_S: return stringify(KEY_S);
        case KEY_D: return stringify(KEY_D);
        case KEY_F: return stringify(KEY_F);
        case KEY_G: return stringify(KEY_G);
        case KEY_H: return stringify(KEY_H);
        case KEY_J: return stringify(KEY_J);
        case KEY_K: return stringify(KEY_K);
        case KEY_L: return stringify(KEY_L);
        case KEY_SEMICOLON: return stringify(KEY_SEMICOLON);
        case KEY_SINGLE_QUOTE: return stringify(KEY_SINGLE_QUOTE);
        case KEY_BACK_TICK: return stringify(KEY_BACK_TICK);
        case KEY_LEFT_SHIFT: return stringify(KEY_LEFT_SHIFT);
        case KEY_BACKSLASH: return stringify(KEY_BACKSLASH);
        case KEY_Z: return stringify(KEY_Z);
        case KEY_X: return stringify(KEY_X);
        case KEY_C: return stringify(KEY_C);
        case KEY_V: return stringify(KEY_V);
        case KEY_B: return stringify(KEY_B);
        case KEY_N: return stringify(KEY_N);
        case KEY_M: return stringify(KEY_M);
        case KEY_COMMA: return stringify(KEY_COMMA);
        case KEY_PERIOD: return stringify(KEY_PERIOD);
        case KEY_SLASH: return stringify(KEY_SLASH);
        case KEY_RIGHT_SHIFT: return stringify(KEY_RIGHT_SHIFT);
        case KEY_KEYPAD_ASTERISK: return stringify(KEY_KEYPAD_ASTERISK);
        case KEY_LEFT_ALT: return stringify(KEY_LEFT_ALT);
        case KEY_SPACE: return stringify(KEY_SPACE);
        case KEY_CAPSLOCK: return stringify(KEY_CAPSLOCK);
        case KEY_F1: return stringify(KEY_F1);
        case KEY_F2: return stringify(KEY_F2);
        case KEY_F3: return stringify(KEY_F3);
        case KEY_F4: return stringify(KEY_F4);
        case KEY_F5: return stringify(KEY_F5);
        case KEY_F6: return stringify(KEY_F6);
        case KEY_F7: return stringify(KEY_F7);
        case KEY_F8: return stringify(KEY_F8);
        case KEY_F9: return stringify(KEY_F9);
        case KEY_F10: return stringify(KEY_F10);
        case KEY_NUMBERLOCK: return stringify(KEY_NUMBERLOCK);
        case KEY_SCROLLLOCK: return stringify(KEY_SCROLLLOCK);
        case KEY_KEYPAD_7: return stringify(KEY_KEYPAD_7);
        case KEY_KEYPAD_8: return stringify(KEY_KEYPAD_8);
        case KEY_KEYPAD_9: return stringify(KEY_KEYPAD_9);
        case KEY_KEYPAD_MINUS: return stringify(KEY_KEYPAD_MINUS);
        case KEY_KEYPAD_4: return stringify(KEY_KEYPAD_4);
        case KEY_KEYPAD_5: return stringify(KEY_KEYPAD_5);
        case KEY_KEYPAD_6: return stringify(KEY_KEYPAD_6);
        case KEY_KEYPAD_PLUS: return stringify(KEY_KEYPAD_PLUS);
        case KEY_KEYPAD_1: return stringify(KEY_KEYPAD_1);
        case KEY_KEYPAD_2: return stringify(KEY_KEYPAD_2);
        case KEY_KEYPAD_3: return stringify(KEY_KEYPAD_3);
        case KEY_KEYPAD_0: return stringify(KEY_KEYPAD_0);
        case KEY_KEYPAD_PERIOD: return stringify(KEY_KEYPAD_PERIOD);
        case KEY_F11: return stringify(KEY_F11);
        case KEY_F12: return stringify(KEY_F12);

        case KEY_PREVIOUS_TRACK: return stringify(KEY_PREVIOUS_TRACK);
        case KEY_NEXT_TRACK: return stringify(KEY_NEXT_TRACK);
        case KEY_KEYPAD_ENTER: return stringify(KEY_KEYPAD_ENTER);
        case KEY_RIGHT_CONTROL: return stringify(KEY_RIGHT_CONTROL);
        case KEY_MUTE: return stringify(KEY_MUTE);
        case KEY_CALCULATOR: return stringify(KEY_CALCULATOR);
        case KEY_PLAY: return stringify(KEY_PLAY);
        case KEY_STOP: return stringify(KEY_STOP);
        case KEY_VOLUME_DOWN: return stringify(KEY_VOLUME_DOWN);
        case KEY_VOLUME_UP: return stringify(KEY_VOLUME_UP);
        case KEY_WWW_HOME: return stringify(KEY_WWW_HOME);
        case KEY_KEYPAD_SLASH: return stringify(KEY_KEYPAD_SLASH);
        case KEY_RIGHT_ALT: return stringify(KEY_RIGHT_ALT);
        case KEY_HOME: return stringify(KEY_HOME);
        case KEY_CURSOR_UP: return stringify(KEY_CURSOR_UP);
        case KEY_PAGE_UP: return stringify(KEY_PAGE_UP);
        case KEY_CURSOR_LEFT: return stringify(KEY_CURSOR_LEFT);
        case KEY_CURSOR_RIGHT: return stringify(KEY_CURSOR_RIGHT);
        case KEY_END: return stringify(KEY_END);
        case KEY_CURSOR_DOWN: return stringify(KEY_CURSOR_DOWN);
        case KEY_PAGE_DOWN: return stringify(KEY_PAGE_DOWN);
        case KEY_INSERT: return stringify(KEY_INSERT);
        case KEY_DELETE: return stringify(KEY_DELETE);
        case KEY_LEFT_GUI: return stringify(KEY_LEFT_GUI);
        case KEY_RIGHT_GUI: return stringify(KEY_RIGHT_GUI);
        case KEY_APPS: return stringify(KEY_APPS);
        case KEY_ACPI_POWER: return stringify(KEY_ACPI_POWER);
        case KEY_ACPI_SLEEP: return stringify(KEY_ACPI_SLEEP);
        case KEY_ACPI_WAKE: return stringify(KEY_ACPI_WAKE);
        case KEY_WWW_SEARCH: return stringify(KEY_WWW_SEARCH);
        case KEY_WWW_FAVORITES: return stringify(KEY_WWW_FAVORITES);
        case KEY_WWW_REFRESH: return stringify(KEY_WWW_REFRESH);
        case KEY_WWW_STOP: return stringify(KEY_WWW_STOP);
        case KEY_WWW_FORWARD: return stringify(KEY_WWW_FORWARD);
        case KEY_WWW_BACK: return stringify(KEY_WWW_BACK);
        case KEY_MY_COMPUTER: return stringify(KEY_MY_COMPUTER);
        case KEY_EMAIL: return stringify(KEY_EMAIL);
        case KEY_MEDIA_SELECT: return stringify(KEY_MEDIA_SELECT);
        default: return "Unknown";
    }
}

const char *key_action_to_string(key_action_t action)
{
    switch (action)
    {
    case KEY_ACTION_RELEASE: return stringify(KEY_ACTION_RELEASE);
    case KEY_ACTION_PRESS: return stringify(KEY_ACTION_PRESS);
    default: return "Unknown";
    }
}
