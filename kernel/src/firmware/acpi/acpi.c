#include "firmware/acpi/madt.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <bootstub.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/def.h>
#endif

#include <firmware/acpi/acpi.h>
#include <firmware/acpi/fadt.h>
#include <misc/debug.h>

#define ACPI_VERSION_2 2

#define ENTRY_TYPE_PROCESSOR_LAPIC          0
#define ENTRY_TYPE_IO_APIC                  1
#define ENTRY_TYPE_IO_APIC_ISO              2
#define ENTRY_TYPE_IO_APIC_NMI_SRC          3
#define ENTRY_TYPE_LAPIC_NMI                4
#define ENTRY_TYPE_LAPIC_ADDR_OVERRIDE      5
#define ENTRY_TYPE_PROCESSOR_LOCAL_2XAPIC   9


extern kernel_context_t *kernel_context;

static rsdp_t *rsdp = NULL;
static xsdp_t *xsdp = NULL;

static bool do_checksum(acpi_sdt_header *header)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < header->length; ++i)
    {
        sum += ((char*)header)[i];
    }

    return sum == 0;
}

static inline void* acpi_get_rsdp(void)
{
    return (void*)kernel_context->rsdp_info.addr;
}

acpi_sdt_header *acpi_find_sdt(const char signature[4])
{   
    if (rsdp->revision >= ACPI_VERSION_2)
    {
        xsdt_t *xsdt = (void*)to_vaddr(xsdp->xsdt_addr);
        if (xsdt->header.length < sizeof(acpi_sdt_header)) return NULL;
        if (!do_checksum(&xsdt->header)) return NULL;

        size_t num_entries = (xsdt->header.length - sizeof(acpi_sdt_header)) / sizeof(uint64_t);
        for (size_t i = 0; i < num_entries; ++i)
        {
            acpi_sdt_header *header = (void*)to_vaddr(xsdt->sdts[i]);
            if (strncmp(header->signature, signature, 4) == 0) return header;
        }
        return NULL;
    } else {
        rsdt_t *rsdt = (void*)to_vaddr(rsdp->rsdt_addr);
        if (rsdt->header.length < sizeof(acpi_sdt_header)) return NULL;
        if (!do_checksum(&rsdt->header)) return NULL;
        
        size_t num_entries = (rsdt->header.length - sizeof(acpi_sdt_header)) / sizeof(uint32_t);
        for (size_t i = 0; i < num_entries; ++i)
        {
            acpi_sdt_header *header = (void*)to_vaddr(rsdt->sdts[i]);   
            if (strncmp(header->signature, signature, 4) == 0) return header;
        }
        return NULL;
    }
}

fadt_t *fadt = NULL;
madt_t *madt = NULL;

volatile uint64_t lapic_addr = 0;
volatile uint64_t ioapic_addr = 0;

volatile uint64_t ioapic_gsi_base = 0;

void acpi_init(void)
{
    rsdp = acpi_get_rsdp(); 
    xsdp = acpi_get_rsdp();
    
    acpi_sdt_header *fadt_header = acpi_find_sdt("FACP");
    if (fadt_header)
    {
        fadt = (fadt_t*)fadt_header;
    }

    acpi_sdt_header *madt_header = acpi_find_sdt("APIC");
    if (madt_header)
    {
        madt = (madt_t*)madt_header;

        lapic_addr = madt->lapic_addr;

        char *curr = (char*)madt + sizeof(madt_t);
        char *end  = (char*)madt + madt->header.length;
        while (curr < end)
        {
            madt_entry_header_t *entry_header = (madt_entry_header_t*)curr; 
            switch (entry_header->entry_type) {
            case ENTRY_TYPE_IO_APIC:
            {
                madt_ioapic_t *entry = (madt_ioapic_t*)entry_header;
                ioapic_addr = entry->io_apic_addr;
                ioapic_gsi_base = entry->global_system_interrupt_base;
                break;
            }
            case ENTRY_TYPE_IO_APIC_ISO:
            {
                madt_ioapic_iso_t *entry = (madt_ioapic_iso_t*)entry_header;
                // TODO: Finish
                break; 
            }
            case ENTRY_TYPE_LAPIC_ADDR_OVERRIDE:
            {
                madt_lapic_addr_override_t *entry = (madt_lapic_addr_override_t*)entry_header;
                lapic_addr = entry->lapic_addr;
                break;   
            }
            default:
                break;  
            }

            curr += entry_header->record_len;
        }
    }

    info(acpi_init, "Initalized!");
}   
