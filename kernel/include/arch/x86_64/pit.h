#pragma once

#include <stdint.h>

void pit_init(uint32_t freq);
void timer_callback();
uint32_t pit_get_current_tick();

