#include <stdarg.h>

#include <mm/vmm/debug.h>

void vmm_debug(vmm_debug_mode_t mode, ...)
{
    va_list args;
    va_start(args, mode);

    switch (mode) {
    case VMM_DEBUG_MODE_PRINT_ADDR_TREE:
    {
        vmm_t *vmm = va_arg(args, vmm_t*); 
    }
    case VMM_DEBUG_MODE_PRINT_SIZE_TREE:
    {

    }
    }

    va_end(args);
}


