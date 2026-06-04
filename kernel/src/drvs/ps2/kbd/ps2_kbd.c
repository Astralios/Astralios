#include "libs/libds/include/ringbuf.h"
#include <kernel.h>
#include <mm/vheap.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/ints/pic.h>
#endif

#include <libs/libinput/include/kbd.h>
#include <drvs/ps2/misc.h>
#include <drvs/ps2/kbd/scancodes.h>
#include <drvs/ps2/kbd/misc.h>
#include <misc/debug.h>
#include <misc/helpers.h>

static uint8_t current_scancode_set;
ringbuf_t key_ringbuf;

void ps2_keyboard_init(void)
{
    current_scancode_set = ps2_keyboard_get_scancode_set(); 
    krnlctx(interrupt_controller)->clear_mask(1);
    info(ps2_keyboard_init, "Initalized!");
    ringbuf_init(&key_ringbuf, 256, sizeof(kbd_ev_t), RINGBUF_MODE_OVERWRITE, vmalloc);
}

void ps2_keyboard_callback(void)
{   
    int sc = ps2_readData();
    if (sc > 0xFF) return;

    kbd_ev_t ev = ps2_decodeFromScancode(sc, current_scancode_set);
    if (ev.keycode != KEY_NONE)
    { 
        ringbuf_write(&key_ringbuf, &ev);
    }
}
