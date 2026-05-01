#include "arch/x86_64/def.h"
#ifdef __ARCH_X86_64__
#include <arch/x86_64/ints/pic.h>
#endif

#include <drvs/ps2/ps2.h>
#include <drvs/ps2/ps2_mouse.h>
#include <drvs/ps2/misc.h>

#include <misc/debug.h>

extern kernel_context_t *kernel_context;

void ps2_mouse_init(void)
{
    ps2_writeCmd(0xD4);
    ps2_writeData(0xF4);

    if (ps2_readData() != PS2_RES_ACK) return;

    kernel_context->interrupt_controller->set_mask(2);
    kernel_context->interrupt_controller->set_mask(12);
    info(ps2_mouse_init, "Initalized!");
}

enum ps2_mouse_packet_bits
{
    YO = 1 << 7,
    XO = 1 << 6,
    YS = 1 << 5,
    XS = 1 << 4,
    BM = 1 << 2,
    BR = 1 << 1,
    BL = 1 << 0,
};

static uint8_t packet_idx = 0;
static uint8_t packet[3] = {0};

int mx = 0;
int my = 0;

void ps2_mouse_callback(void)
{
    uint8_t data = ps2_readData();
    if (data == 0xFF) return;

    packet[packet_idx++] = data;

    if (packet_idx == 3)
    {
        bool xs = packet[0] & (XS);
        bool ys = packet[0] & (YS);
        int x = packet[1];
        int y = packet[2];
        if (xs) x = -x;
        if (ys) y = -y;
        mx += x;
        my += y;
    
        srdebug(ps2_mouse_callback, "x: %d, y: %d", mx, my);
        packet_idx = 0;
    }
}

