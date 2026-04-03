#include "arch/x86_64/pit.h"
#include "arch/x86_64/io.h"
#include "arch/x86_64/interrupts/controller/pic.h"
#include "devices/tty.h"
#include "misc/debug.h"
#include <stdint.h>

static uint32_t tick = 0;

void timer_callback()
{
    tick++;
    pic_send_eoi(0);
}

uint32_t pit_get_current_tick()
{
    return tick;
}

void pit_init(uint32_t freq)
{    
    uint32_t div = 1193180 / freq;
    outb(0x43, 0x36);
    uint8_t l = (uint8_t)(div & 0xFF);
    uint8_t h = (uint8_t)((div >> 8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
    irq_clear_mask(0);
}
