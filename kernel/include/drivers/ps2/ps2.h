#pragma once

#include <stdint.h>

#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_CMD     0x64

#define PS2_RES_ACK 0xFA

#define PS2_DISABLE_PORT1   0xAD
#define PS2_DISABLE_PORT2   0xA7

int ps2_wait_write(void);
int ps2_wait_read(void);
void ps2_init();
void detect_ps2_device_type();
int ps2_read_data(void);
int ps2_write_data(uint8_t v);

