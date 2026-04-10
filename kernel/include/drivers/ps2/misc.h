#pragma once

#include <stdint.h>

int     ps2_readData(void);
int     ps2_writeData(uint8_t v);
int     ps2_writeCmd(uint8_t cmd);
int     ps2_writeDataToPort2(uint8_t data);
uint8_t ps2_readStatus(void);
uint8_t ps2_readControllerConfig(void);
int8_t  ps2_keyboard_get_scancode_set(void);