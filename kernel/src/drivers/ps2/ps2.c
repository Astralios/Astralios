#include "drivers/ps2/ps2.h"
#include "arch/x86_64/io.h"
#include "devices/tty.h"
#include "misc/debug.h"
#include "misc/err.h"
#include <stdint.h>
#include <stdbool.h>

int ps2_wait_write(void)
{
    size_t tries = 100000;
    while ((inb(PS2_STATUS) & 0b10) != 0 && (--tries));
    return tries == 0 ? -TIMEOUT : SUCCESS;
}

int ps2_wait_read(void)
{
    size_t tries = 100000;
    while ((inb(PS2_STATUS) & 0b1) == 0 && (--tries));
    return tries == 0 ? -TIMEOUT : SUCCESS;
}

int ps2_write_data(uint8_t v)
{
    int e;
    if ((e = ps2_wait_write()) < 0) return e;
    outb(PS2_DATA, v);
    return SUCCESS;
}

int ps2_read_data(void)
{
    int e;
    if ((e = ps2_wait_read()) < 0) return e;
    return inb(PS2_DATA);
}

int ps2_send_cmd(uint8_t cmd)
{
    int e;
    if ((e = ps2_wait_write()) < 0) return e;
    outb(PS2_CMD, cmd);
    return SUCCESS;
}

uint8_t ps2_read_status(void)
{
    return inb(PS2_STATUS);
}

int ps2_write_data_to_port2(uint8_t v)
{
    int e;
    if ((e = ps2_send_cmd(0xD4)) < 0) return e;
    if ((e = ps2_write_data(v)) < 0) return e;
    return e;
}

void ps2_init()
{
    // TODO: USB Init
    // TODO: Check if PS2 controller even exists
    ps2_send_cmd(PS2_DISABLE_PORT1);
    ps2_send_cmd(PS2_DISABLE_PORT2);

    while (ps2_read_status() & 0b1) ps2_read_data();
    
    ps2_send_cmd(0x20);
    uint8_t v = ps2_read_data();
    v &= ~(1 << 0);
    v &= ~(1 << 1);
    v &= ~(1 << 6);
    
    ps2_send_cmd(0x60);
    ps2_write_data(v);    

    ps2_send_cmd(0xAA);
    v = ps2_read_data();
    if (v == 0x55)
    {
        debug("Correct");
    } else {
        debug("Incorrect: %x", v); 
    }

    ps2_send_cmd(0xA8);
    ps2_send_cmd(0x20);
    v = ps2_read_data();
    bool dual_channel = false;
    if ((v & (1 << 5)) == 0) dual_channel = true;
    if (dual_channel)
    {
        ps2_send_cmd(0xA7);
        v &= ~(1 << 1);
        v &= ~(1 << 5);
        ps2_send_cmd(0x60);
        ps2_write_data(v);    
    }

    ps2_send_cmd(0xAB);
    v = ps2_read_data();
    debug("0xAB Test: %x", v);
    ps2_send_cmd(0xA9);
    v = ps2_read_data();
    debug("0xA9 Test: %x", v);

    ps2_send_cmd(0xAE);
    ps2_send_cmd(0xA8);
    ps2_send_cmd(0x20);
    v = ps2_read_data();

    v |= (1 << 0);

    ps2_send_cmd(0x60);
    ps2_write_data(v);    

    ps2_write_data(0xFF);
    v = ps2_read_data();
    debug("%x", v);
}


void detect_ps2_device_type()
{
    ps2_write_data(0xF5);
    int v = ps2_read_data();
    if (v != PS2_RES_ACK) return;
    ps2_write_data(0xF2);
    v = ps2_read_data();
    if (v != PS2_RES_ACK) return;

    int first_byte = ps2_read_data();
    int second_byte = ps2_read_data();
    debug("f: %x; s: %x", first_byte, second_byte);
        
    ps2_write_data(0xF4);
}
