#pragma once
#include <devs/serial.h>
#define printk(fmt, ...) srprintf(fmt, ##__VA_ARGS__)
