#ifndef K_ACPI_H
#define K_ACPI_H

#include "include/error.h"
#include "include/types.h"

#define K_ACPI_RSDP_SIGNATURE	"RSD PTR "

/* ACPI RSDP. */
struct k_acpi_rsdp {
	__u8	signature[8];
	__u8	checksum;
	__u8	oemid[6];
	__u8	revision;
	__u32	rsdt_address;
	__u32	length;
	__u64	xsdt_address;
	__u8	extended_checksum;
	__u8	reserved[3];
} __attribute__((packed));

/* ACPI Generic Address Structure. */
#define K_ACPI_ADDRESS_SPACE_ID_SYSTEM_MEMORY	0x0

struct k_acpi_gas {
	__u8	space_id;
	__u8	register_bit_width;
	__u8	register_bit_offset;
	__u8	access_size;
	__u64	address;
} __attribute__((packed));

/* ACPI SDT. */
struct k_acpi_sdt {
	__u8	signature[4];
	__u32	length;
	__u8	revision;
	__u8	checksum;
	__u8	oemid[6];
	__u8	oem_table_id[8];
	__u32	oem_revision;
	__u8	creator_id[4];
	__u32	creator_revision;
} __attribute__((packed));

/* ACPI RSDT. */
#define K_ACPI_RSDT_SIGNATURE	"RSDT"

struct k_acpi_rsdt {
	struct k_acpi_sdt sdt;
	__u32	entries[0];
} __attribute__((packed));

/* ACPI MADT. */
#define K_ACPI_MADT_SIGNATURE	"APIC"

#define K_ACPI_MADT_FLAGS_PCAT_COMPAT	(1 << 0)

struct k_acpi_madt {
	struct k_acpi_sdt sdt;
	__u32	lapic_address;
	__u32	flags;
	__u8	entries[0];
} __attribute__((packed));

/* ACPI MADT entries. */
#define K_ACPI_MADT_LAPIC		0x0
#define K_ACPI_MADT_IOAPIC		0x1
#define K_ACPI_MADT_INTERRUPT_OVERRIDE	0x2
#define K_ACPI_MADT_LAPIC_NMI		0x4

/* ACPI Local APIC. */
#define K_ACPI_LAPIC_FLAGS_ENABLED	(1 << 0)

struct k_acpi_lapic {
	__u8	type;
	__u8	length;
	__u8	acpi_processor_id;
	__u8	apic_id;
	__u32	flags;
} __attribute__((packed));

/* ACPI I/O APIC. */
struct k_acpi_ioapic {
	__u8	type;
	__u8	length;
	__u8	apic_id;
	__u8	reserved;
	__u32	ioapic_address;
	__u32	global_irq_base;
} __attribute__((packed));

/* ACPI Interrupt Source Override. */
struct k_acpi_interrupt_override {
	__u8	type;
	__u8	length;
	__u8	bus;
	__u8	source_irq;
	__u32	global_irq;
	__u16	flags;
} __attribute__((packed));

/* ACPI Local APIC NMI. */
struct k_acpi_lapic_nmi {
	__u8	type;
	__u8	length;
	__u8	acpi_processor_id;
	__u16	flags;
	__u8	lint;
} __attribute__((packed));

/* ACPI HPET. */
#define K_ACPI_HPET_SIGNATURE	"HPET"

struct k_acpi_hpet {
	struct k_acpi_sdt sdt;
	__u32	id;
	struct k_acpi_gas address;
	__u8	number;
	__u16	clock_tick;
	__u8	page_protection;
} __attribute__((packed));

void k_acpi_get_info(void);

#endif

