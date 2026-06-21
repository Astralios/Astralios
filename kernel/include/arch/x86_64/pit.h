#pragma once

#include <kernel.h>
#include <stdint.h>

extern int_ctrl_t pic;

void pit_init(uint32_t freq);
void timer_callback();
uint64_t pit_get_current_tick();
void pit_sleep_ms(uint64_t ms);


