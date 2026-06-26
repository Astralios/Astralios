#pragma once

#define CMOS_REGISTER_SECONDS   0x00
#define CMOS_REGISTER_MINUTES   0x02
#define CMOS_REGISTER_HOURS     0x04
#define CMOS_REGISTER_WEEKDAY   0x06
#define CMOS_REGISTER_MONTH_DAY 0x07
#define CMOS_REGISTER_MONTH     0x08
#define CMOS_REGISTER_YEAR      0x09

typedef struct {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
} rtc_time_t;

void rtc_time_get(rtc_time_t *now);

