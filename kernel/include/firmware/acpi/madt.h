#pragma once

#include <stdint.h>
#include <firmware/acpi/acpi.h>

#define ACPI_MADT_SIG "APIC"

#define MADT_ENTRY_TYPE_PROCESSOR_LAPIC          0
#define MADT_ENTRY_TYPE_IO_APIC                  1
#define MADT_ENTRY_TYPE_IO_APIC_ISO              2
#define MADT_ENTRY_TYPE_IO_APIC_NMI_SRC          3
#define MADT_ENTRY_TYPE_LAPIC_NMI                4
#define MADT_ENTRY_TYPE_LAPIC_ADDR_OVERRIDE      5
#define MADT_ENTRY_TYPE_PROCESSOR_LOCAL_2XAPIC   9

typedef struct 
{
    uint8_t entry_type;
    uint8_t record_len;
    union
    {
        struct 
        {
            uint8_t  processor_id;
            uint8_t  id;
            uint32_t flags;
        } __attribute__((packed)) lapic;
        struct 
        {
            uint8_t  id;
            uint8_t  reserved;
            uint32_t ioapic_addr;
            uint32_t gsi_base;
        } __attribute__((packed)) ioapic;
        struct
        {
            uint8_t  bus_src;
            uint8_t  irq_src;
            uint32_t gsi;
            uint16_t flags;
        } __attribute__((packed)) ioapic_iso;
        struct
        {
            uint8_t  nmi_src;
            uint8_t  reserved;
            uint16_t flags;
            uint32_t gsi;
        } __attribute__((packed)) ioapic_nmi;
        struct
        {
            uint8_t  processor_id;
            uint16_t flags;
            uint8_t  lint;
        }  __attribute__((packed)) lapic_nmi;
        struct
        {
            uint16_t reserved;
            uint64_t lapic_addr;
        } __attribute__((packed)) lapic_addr_override;
        struct
        {
            uint16_t reserved;
            uint32_t processors_l2xapic_id;
            uint32_t flags;
            uint32_t acpi_id;
        } __attribute((packed)) lx2apic;
    };  
} __attribute__((packed)) madt_entry_t;

typedef struct madt_t {
    acpi_sdt_header header;
    uint32_t lapic_addr;
    uint32_t flags;
} __attribute__((packed)) madt_t;

