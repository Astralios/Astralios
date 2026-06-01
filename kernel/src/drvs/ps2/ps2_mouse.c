#include <kernel.h>
#include <stdbool.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/ints/pic.h>
#endif

#include <drvs/ps2/ps2.h>
#include <drvs/ps2/ps2_mouse.h>
#include <drvs/ps2/misc.h>

#include <misc/debug.h>

void ps2_mouse_init(void)
{
    ps2_writeCmd(0xD4);
    ps2_writeData(0xF4);

    if (ps2_readData() != PS2_RES_ACK) return;

    krnl_ctx.interrupt_controller->clear_mask(12);
    info(ps2_mouse_init, "Initalized!");
}

static uint8_t packet_idx = 0;
static uint8_t packet[3] = {0};

extern int x;
extern int y;
extern bool left_click;

void ps2_mouse_callback(void)
{
    int data = ps2_readData();
    if (data < 0) return;
    
    packet[packet_idx++] = data;

    if (packet_idx == 3)
    {

        int dx = (int)packet[1];
        int dy = (int)packet[2];
    
        if (packet[0] & (1 << 4)) dx |= 0xFFFFFF00;
        if (packet[0] & (1 << 5)) dy |= 0xFFFFFF00;

        x += dx;
        y -= dy;

        left_click = (packet[0] & 1); 

        packet_idx = 0;
    }
}

