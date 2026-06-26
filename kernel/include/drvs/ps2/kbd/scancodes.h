#pragma once

#include <libinput/include/kbd.h>
#include <stdint.h>

kbd_ev_t ps2_decodeFromScancode(uint8_t sc, uint8_t set);

