#pragma once

#include <string.h>

#define CPUID_GET_VENDOR_ID     0
#define CPUID_GET_CPU_FEATURES  1

// TODO: Add more

static inline void get_cpu_vendor_id(char buf[13])
{
    unsigned int eax = 0, ebx, ecx, edx;

    __asm__ volatile(
        "cpuid"
        : "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(eax));

    char vendor[13] = {0};
    *(unsigned int *)&vendor[0] = ebx;
    *(unsigned int *)&vendor[4] = edx;
    *(unsigned int *)&vendor[8] = ecx;
    memcpy(buf, vendor, 13);
}
