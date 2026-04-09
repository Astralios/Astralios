#include "io/kbd.h"
#include "vendor/ringbuf.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#ifdef __ARCH_X86_64__
#include "arch/x86_64/interrupts/controller/pic.h"
#endif

#include "drivers/ps2/ps2_keyboard.h"
#include "drivers/ps2/ps2.h"
#include "misc/debug.h"
#include "misc/bitflags.h"

static int scancode_set_2[] = {
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
    KEY_NONE, KEY_NONE, KEY_NONE, KEY_F7
};

static char normal_char_keymap[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    /* Maybe not */ 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', 0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', 
    '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0
};

static char shift_char_keymap[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    /* Maybe not */ 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}', 0, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
    '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', 
    '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0
};

ringbuf_t(char, char_ringbuf_t);

static char keybuf[256] = {0};
static char_ringbuf_t keyringbuf = { .buf = keybuf, .cap = sizeof(keybuf), .head = 0, .tail = 0 };

int8_t ps2_keyboard_get_scancode_set(void)
{
    ps2_write_data(0xF0);
    if (ps2_read_data() != PS2_RES_ACK)
    {
        return -1;
    }

    ps2_write_data(0);
    if (ps2_read_data() != PS2_RES_ACK)
    {
        return -1;
    }

    return ps2_read_data();
}

static char* controller_config_to_str[] = {
    "FIRST PORT INTERRUPT",
    "SECOND PORT INTERRUPT",
    "SYSTEM FLAG",
    "SHOULD BE ZERO",
    "FIRST PORT CLOCK",
    "SECOND PORT CLOCK",
    "FIRST PORT TRANSLATION",
    "MUST BE ZERO",
};

void ps2_keyboard_init(void)
{
    irq_clear_mask(1);
}


static bool released = false;
static bool shift = false;
static bool ctrl = false;
static bool caps = false; 

void ps2_keyboard_callback(void)
{ 
    int sc = ps2_read_data();

    if (sc == 0xF0) 
    {
        released = true;
        return;
    }

    switch (sc) {
    case 0x12:
    case 0x59:
        shift = !released;
        break;
    }

    if (sc != 0 && sc <= 131)
    {
        int key = scancode_set_2[sc];
        char c;
        if (!shift) c = normal_char_keymap[key];
        else c = shift_char_keymap[key];
        if (c != 0) srdebug(ps2_keyboard_callback, "%c", c);
    }

    kbd_event_t ev = { 
        .scancode = sc,
        .action = released ? KEY_ACTION_RELEASE : KEY_ACTION_PRESS,
        .mode = 
            (shift ? KEY_MODE_SHIFT : 0) |
            (ctrl  ? KEY_MODE_CTRL : 0)  
    };
    
    released = false;
}

