#include <stdint.h>
#ifdef __ARCH_X86_64__
#include <arch/x86_64/hw/io.h>
#endif

#include <drvs/cmos.h>

// 0 - Enable
// 1 - Disable
#define NMI_BIT 0

#define CMOS_PORT 0x71

void cmos_reg_select(int reg_num)
{
    outb(0x70, (NMI_BIT << 7) | reg_num);  
}

uint8_t cmos_read(int reg_num)
{
    cmos_reg_select(reg_num);
    return inb(CMOS_PORT);
}
