#pragma once

#include <stdint.h>
#include <firmware/acpi/acpi.h>

typedef struct madt_entry_header_t 
{
    uint8_t entry_type;
    uint8_t record_len;
} __attribute__((packed)) madt_entry_header_t;

typedef struct madt_processor_lapic_t 
{
    madt_entry_header_t header;
    uint8_t  acpi_processor_id;
    uint8_t  apic_id;
    uint32_t flags;
} __attribute__((packed)) madt_processor_lapic_t;

typedef struct madt_ioapic_t 
{
    madt_entry_header_t header;
    uint8_t  io_apic_id;
    uint8_t  reserved;
    uint32_t io_apic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__((packed)) madt_ioapic_t;

typedef struct madt_ioapic_iso_t
{
    madt_entry_header_t header;
    uint8_t  bus_source;
    uint8_t  irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed)) madt_ioapic_iso_t;

typedef struct madt_ioapic_nmi_src_t
{
    madt_entry_header_t header;
    uint8_t  nmi_src;
    uint8_t  reserved;
    uint16_t flags;
    uint32_t global_system_interrupt;
} __attribute__((packed)) madt_io_apic_nmi_src_t;

typedef struct madt_lapic_nmi_t
{
    madt_entry_header_t header;
    uint8_t  acpi_processor_id;
    uint16_t flags;
    uint8_t  lint;
} __attribute__((packed)) madt_lapic_nmi_t;

typedef struct madt_lapic_addr_override_t
{
    madt_entry_header_t header;
    uint16_t reserved;
    uint64_t lapic_addr;
} __attribute__((packed)) madt_lapic_addr_override_t;

typedef struct madt_processor_lx2apic_t
{
    madt_entry_header_t header;
    uint16_t reserved;
    uint32_t processors_l2xapic_id;
    uint32_t flags;
    uint32_t acpi_id;
} __attribute__((packed)) madt_processor_lx2apic_t;

typedef struct madt_t {
    acpi_sdt_header header;
    uint32_t lapic_addr;
    uint32_t flags;
} __attribute__((packed)) madt_t;

