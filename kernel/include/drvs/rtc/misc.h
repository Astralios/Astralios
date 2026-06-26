#pragma once

#include <drvs/rtc/rtc.h>

#define printk_rtc_time(time) _printk_rtc_time(#time, &time)

void _printk_rtc_time(const char *prefix, rtc_time_t *now);

