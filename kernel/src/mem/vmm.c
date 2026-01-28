#include "mem/vmm.h"
#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#include "arch/x86_64/mem/paging.h"
#endif

extern page_table_t *pml4;

vaddr_t vmm_alloc(vmm_t *vmm, char *name, size_t len, int flags)
{
    if (len == 0) return 0;
    if (name == NULL) name = "Unnamed";
    


}
