#include "arch/x86_64/interrupts/controller/pic.h"
#include "drivers/ps2/ps2.h"
#include "misc/debug.h"
#include "misc/bitflags.h"
#include "drivers/ps2/ps2_mouse.h"

void ps2_mouse_init(void)
{
    ps2_write_cmd(PS2_READ_CONTROLLER_CONFIG);
    uint8_t config = ps2_read_data();

    // config |= PS2_SECOND_PORT_INTERRUPT_CONFIG;
    config |= PS2_FIRST_PORT_INTERRUPT_CONFIG;
    config &= ~(PS2_FIRST_PORT_CLOCK_CONFIG | PS2_SECOND_PORT_CLOCK_CONFIG);

    ps2_write_cmd(PS2_WRITE_CONTROLLER_CONFIG);
    ps2_write_data(config);

    ps2_write_cmd(0xA8);

    ps2_write_cmd(0xD4);
    ps2_write_data(0xF4);

    if (ps2_read_data() != PS2_RES_ACK)
        return;

    irq_clear_mask(2);
    irq_clear_mask(12);
}

void ps2_mouse_callback(void)
{
    uint8_t data = ps2_read_data();
    srdebug(ps2_mouse_callback, "%x", data);
}

