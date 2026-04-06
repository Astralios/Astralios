#pragma once

#include <stdint.h>

int8_t ps2_keyboard_get_scancode_set(void);
void ps2_keyboard_callback(void);
void ps2_keyboard_init(void);

