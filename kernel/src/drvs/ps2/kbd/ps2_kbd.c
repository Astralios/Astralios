#include <bootstub.h>

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

extern kernel_context_t *kernel_context;

static uint8_t current_scancode_set;


void ps2_keyboard_init(void)
{
    current_scancode_set = ps2_keyboard_get_scancode_set(); 
    kernel_context->interrupt_controller->clear_mask(1);
    info(ps2_keyboard_init, "Initalized!");
}

void ps2_keyboard_callback(void)
{   
    int sc = ps2_readData();
    if (sc > 0xFF) return;

    kbd_raw_data_t data = ps2_decodeFromScancode(sc, current_scancode_set);

    if (data.keycode != KEY_NONE)
    {
        srdebug(ps2_keyboard_callback, "%s: %s", key_to_string(data.keycode), key_action_to_string(data.action));
    }
}

