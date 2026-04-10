#include <stddef.h>
#include "drivers/ps2/ps2.h"
#include "misc/err.h"
#include "arch/x86_64/io.h"

static int ps2_waitWrite(void)
{
    size_t tries = 100000;
    while ((inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_STATUS) != PS2_BUFFER_EMPTY && (--tries));
    return tries == 0 ? -TIMEOUT : SUCCESS;
}

static int ps2_waitRead(void)
{
    size_t tries = 100000;
    while ((inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_STATUS) == PS2_BUFFER_EMPTY && (--tries));
    return tries == 0 ? -TIMEOUT : SUCCESS;
}

int ps2_writeData(uint8_t data)
{
    int e;
    if ((e = ps2_waitWrite()) < 0) return e;
    outb(PS2_DATA_PORT, data);
    return e;
}

int ps2_readData(void)
{
    int e;
    if ((e = ps2_waitRead()) < 0) return e;
    return inb(PS2_DATA_PORT);
}

int ps2_writeCmd(uint8_t cmd)
{
    int e;
    if ((e = ps2_waitWrite()) < 0) return e;
    outb(PS2_CMD_PORT, cmd);
    return e;
}

int ps2_writeDataToPort2(uint8_t data)
{
    int e;
    if ((e = ps2_writeCmd(0xD4)) < 0)  return e;
    if ((e = ps2_writeData(data)) < 0) return e;
    return e;
}

uint8_t ps2_readStatus(void)
{
    return inb(PS2_STATUS_PORT);
}

uint8_t ps2_readControllerConfig(void)
{
    ps2_writeCmd(PS2_READ_CONTROLLER_CONFIG);
    return ps2_readData();
}

int8_t ps2_keyboard_get_scancode_set(void)
{
    ps2_writeData(0xF0);
    if (ps2_readData() != PS2_RES_ACK)
    {
        return -1;
    }

    ps2_writeData(0);
    if (ps2_readData() != PS2_RES_ACK)
    {
        return -1;
    }

    return ps2_readData();
}

static char* controller_config_to_str[] = {
    "FIRST PORT INTERRUPT",
    "SECOND PORT INTERRUPT",
    "SYSTEM FLAG",
    "SHOULD BE ZERO",
    "FIRST PORT CLOCK",
    "SECOND PORT CLOCK",
    "FIRST PORT TRANSLATION",
    "MUST BE ZERO",
};