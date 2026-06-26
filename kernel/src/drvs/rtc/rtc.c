#include "drvs/cmos.h"
#include <drvs/rtc/rtc.h>
#include <stdint.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/hw/io.h>
#endif

static int bcd_to_int(int bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0xF));
}

void rtc_time_get(rtc_time_t *now)
{
    uint8_t tm_sec =  cmos_read(CMOS_REGISTER_SECONDS);
    uint8_t tm_min =  cmos_read(CMOS_REGISTER_MINUTES);
    uint8_t tm_hour = cmos_read(CMOS_REGISTER_HOURS);
    uint8_t tm_mday = cmos_read(CMOS_REGISTER_MONTH_DAY);
    uint8_t tm_mon =  cmos_read(CMOS_REGISTER_MONTH);
    uint8_t tm_year = cmos_read(CMOS_REGISTER_YEAR);
    uint8_t tm_wday = cmos_read(CMOS_REGISTER_WEEKDAY);

    *now = (rtc_time_t)
    {
        .tm_sec  = bcd_to_int(tm_sec),
        .tm_min  = bcd_to_int(tm_min),
        .tm_hour = bcd_to_int(tm_hour),
        .tm_mday = bcd_to_int(tm_mday),
        .tm_mon  = bcd_to_int(tm_mon),
        .tm_year = bcd_to_int(tm_year),
        .tm_wday = bcd_to_int(tm_wday),
    };
}

