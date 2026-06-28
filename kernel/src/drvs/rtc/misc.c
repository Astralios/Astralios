#include "misc/debug.h"
#include <drvs/rtc/misc.h>

void _printk_rtc_time(const char *prefix, rtc_time_t *now)
{
    debug("%s: secs: %d, mins: %d, hours: %d, wday: %d, mday: %d, mon: %d, year: %d", prefix, now->tm_sec, now->tm_min, now->tm_hour, now->tm_wday, now->tm_mday, now->tm_mon, now->tm_year);
}

