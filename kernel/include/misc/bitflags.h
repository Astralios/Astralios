#pragma once

#include <stdint.h>
#include "devices/serial.h"

static inline void print_flags(uint64_t flags, uint8_t num_bits, char **str_table)
{
    for (int i = 0; i < num_bits; i++)
    {
        bool bit = (flags >> i) & 0b1;
        bool next_bit = (flags >> (i + 1)) & 0b1;
        if (bit) srprintf("%s", str_table[i]);
        if (next_bit && (i != 0 || bit)) srprintf(" | ");
    }
    srprintf("\n");
}
