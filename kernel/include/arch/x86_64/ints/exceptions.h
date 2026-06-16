#pragma once
#include <stdint.h>

typedef enum 
{
    EXCEPTION_DIVISION_ERROR                    = 0x00,
    EXCEPTION_DEBUG                             = 0x01,
    EXCEPTION_NON_MASKABLE_INTERRUPT            = 0x02,
    EXCEPTION_BREAKPOINT                        = 0x03,
    EXCEPTION_OVERFLOW                          = 0x04,
    EXCEPTION_BOUND_RANGE_EXCEEDED              = 0x05,
    EXCEPTION_INVALID_OPCODE                    = 0x06,
    EXCEPTION_DEVICE_NOT_AVAILABLE              = 0x07,
    EXCEPTION_DOUBLE_FAULT                      = 0x08,
    EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN       = 0x09,
    EXCEPTION_INVALID_TSS                       = 0x0A,
    EXCEPTION_SEGMENT_NOT_PRESENT               = 0x0B,
    EXCEPTION_STACK_SEGMENT_FAULT               = 0x0C,
    EXCEPTION_GENERAL_PROTECTION_FAULT          = 0x0D,
    EXCEPTION_PAGE_FAULT                        = 0x0E,
    EXCEPTION_X87_FLOATING_POINT_EXCEPTION      = 0x10,
    EXCEPTION_ALIGNMENT_CHECK                   = 0x11,
    EXCEPTION_MACHINE_CHECK                     = 0x12,
    EXCEPTION_SIMD_FLOATING_POINT_EXCEPTION     = 0x13,
    EXCEPTION_VIRTUALIZATION_EXCEPTION          = 0x14,
    EXCEPTION_CONTROL_PROTECTION_EXCEPTION      = 0x15,
    EXCEPTION_HYPERVISOR_INJECTION_EXCEPTION    = 0x1C,
    EXCEPTION_VMM_COMMUNICATION_EXCEPTION       = 0x1D,
    EXCEPTION_SECURITY_EXCEPTION                = 0x1E,
} exception_t;

typedef struct
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t exception_code;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) exception_frame_t;


