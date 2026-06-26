#pragma once
#include <stdint.h>

uint8_t cmos_read(int reg_num);
void cmos_reg_select(int reg_num);
