#pragma once

#include "libs/libinput/include/kbd.h"
#include <stdint.h>

kbd_raw_data_t ps2_decodeFromScancode(uint8_t sc, uint8_t set);