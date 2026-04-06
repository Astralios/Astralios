#include "drivers/ps2/ps2.h"
#include "arch/x86_64/io.h"
#include "devices/tty.h"
#include "misc/debug.h"
#include "misc/err.h"
#include "misc/bitflags.h"
#include <stdint.h>
#include <stdbool.h>

static int ps2_wait_write(void)
{
    size_t tries = 100000;
    while ((inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_STATUS) != PS2_BUFFER_EMPTY && (--tries));
    return tries == 0 ? -TIMEOUT : SUCCESS;
}

static int ps2_wait_read(void)
{
    size_t tries = 100000;
    while ((inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_STATUS) == PS2_BUFFER_EMPTY && (--tries));
    return tries == 0 ? -TIMEOUT : SUCCESS;
}

int ps2_write_data(uint8_t data)
{
    int e;
    if ((e = ps2_wait_write()) < 0) return e;
    outb(PS2_DATA_PORT, data);
    return e;
}

int ps2_read_data(void)
{
    int e;
    if ((e = ps2_wait_read()) < 0) return e;
    return inb(PS2_DATA_PORT);
}

int ps2_write_cmd(uint8_t cmd)
{
    int e;
    if ((e = ps2_wait_write()) < 0) return e;
    outb(PS2_CMD_PORT, cmd);
    return e;
}

uint8_t ps2_read_status(void)
{
    return inb(PS2_STATUS_PORT);
}

int ps2_write_data_to_port2(uint8_t v)
{
    int e;
    if ((e = ps2_write_cmd(0xD4)) < 0) return e;
    if ((e = ps2_write_data(v)) < 0) return e;
    return e;
}

uint8_t ps2_readControllerConfig(void)
{
    ps2_write_cmd(PS2_READ_CONTROLLER_CONFIG);
    return ps2_read_data();
}

void ps2_init(void)
{
    // TODO: USB Init
    // TODO: Check if PS2 controller even exists
    ps2_write_cmd(PS2_DISABLE_FIRST_PORT);
    ps2_write_cmd(PS2_DISABLE_SECOND_PORT);
    
    while ((ps2_read_status() & PS2_OUTPUT_BUFFER_STATUS) != PS2_BUFFER_EMPTY) ps2_read_data();
    
    ps2_write_cmd(PS2_READ_CONTROLLER_CONFIG);
    
    ps2_controller_config_t data = ps2_read_data();
    data &= ~(PS2_FIRST_PORT_INTERRUPT_CONFIG);
    data &= ~(PS2_SECOND_PORT_INTERRUPT_CONFIG);
    data &= ~(PS2_FIRST_PORT_TRANSLATION_CONFIG);
    
    ps2_write_cmd(PS2_WRITE_CONTROLLER_CONFIG);
    ps2_write_data(data);    

    ps2_write_cmd(PS2_TEST_CONTROLLER);
    data = ps2_read_data();
    if (data != PS2_TEST_CONTROLLER_SUCCESS) return;

    ps2_write_cmd(PS2_ENABLE_SECOND_PORT);
    ps2_write_cmd(PS2_READ_CONTROLLER_CONFIG);
    data = ps2_read_data();

    bool dual_channel = false;
    if ((data & (PS2_SECOND_PORT_CLOCK_CONFIG)) == 0) dual_channel = true;
    if (dual_channel)
    {
        ps2_write_cmd(PS2_DISABLE_SECOND_PORT);
        data &= ~(PS2_SECOND_PORT_INTERRUPT_CONFIG);
        data &= ~(PS2_SECOND_PORT_CLOCK_CONFIG);
        ps2_write_cmd(PS2_WRITE_CONTROLLER_CONFIG);
        ps2_write_data(data);    
    }

    ps2_write_cmd(PS2_TEST_FIRST_PORT);
    data = ps2_read_data();
    if (data != PS2_TEST_PORT_SUCCESS) return;
    ps2_write_cmd(PS2_TEST_SECOND_PORT);
    data = ps2_read_data();
    if (data != PS2_TEST_PORT_SUCCESS) return;

    ps2_write_cmd(PS2_ENABLE_FIRST_PORT);
    ps2_write_cmd(PS2_ENABLE_SECOND_PORT);
    
    ps2_write_cmd(PS2_READ_CONTROLLER_CONFIG);
    data = ps2_read_data();

    data |= (PS2_FIRST_PORT_INTERRUPT_CONFIG);

    ps2_write_cmd(PS2_WRITE_CONTROLLER_CONFIG);
    ps2_write_data(data);    

    ps2_write_data(0xFF);
    data = ps2_read_data();
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
