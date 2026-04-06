#pragma once
extern void fpu_enable(void);

static inline float fcosf(float x)
{
    float result;
    __asm__ volatile(
        "flds %1"
        "fcos"
        "fstps %0"
        : "=m"(result)
        : "m"(x));
    return result;
}

static inline float fsinf(float x)
{
    float result;
    __asm__ volatile(
        "flds %1"
        "fsin"
        "fstps %0"
        : "=m"(result)
        : "m"(x)
    );
    return result;
}
