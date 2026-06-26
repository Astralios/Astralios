#include <stdarg.h>

#include <mm/vmm/debug.h>

void vmm_debug(vmm_debug_mode_t mode, ...)
{
    va_list args;
    va_start(args, mode);

    va_end(args);
}


