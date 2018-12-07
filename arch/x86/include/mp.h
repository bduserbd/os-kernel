#ifndef K_MP_H
#define K_MP_H

#include "kernel/include/error.h"
#include "kernel/include/types.h"

#define K_SMP_SIGNATURE	0x5f504d5f

struct k_mp_floating_pointer {
	__u32	signature;
	__u32	configuration_table_pointer;
	__u8	length;
	__u8	spec_rev;
	__u8	checksum;
	__u8	configuration_type;
	__u32	features;
} __attribute__((packed));

#define K_SMP_CONFIGURATION_TABLE_SIGNATURE	0x504d4350

struct k_mp_configuration_table {
	__u32	signature;
	__u16	base_table_length;
	__u8	spec_rev;
	__u8	checksum;
	__u8	oem_id[8];
	__u8	product_id[12];
	__u32	oem_table_pointer;
	__u16	oem_table_size;
	__u16	entry_count;
	__u32	lapic_address;
	__u16	extended_table_length;
	__u8	extended_table_checksum;
	__u8	reserved;
} __attribute__((packed));

enum {
	K_SMP_ENTRY_PROCESSOR		= 0,
	K_SMP_ENTRY_BUS			= 1,
	K_SMP_ENTRY_IOAPIC		= 2,
	K_SMP_ENTRY_IO_INTERRUPT	= 3,
	K_SMP_ENTRY_LOCAL_INTERRUPT	= 4,
};

struct k_mp_processor_entry {
	__u8	entry_type;
	__u8	lapic_id;
	__u8	lapic_version;
	__u8	cpu_flags;
	__u32	cpu_signature;
	__u32	feature_flags;
	__u32	reserved[2];
} __attribute__((packed));

struct k_mp_bus_entry {
	__u8	entry_type;
	__u8	bus_id;
	__u8	bus_type_string[6];
} __attribute__((packed));

struct k_mp_ioapic_entry {
	__u8	entry_type;
	__u8	ioapic_id;
	__u8	ioapic_version;
	__u8	ioapic_flags;
	__u32	ioapic_address;
} __attribute__((packed));

struct k_mp_io_interrupt_entry {
	__u8	entry_type;
	__u8	interrupt_type;
	__u8	io_interrupt_flags;
	__u8	source_bus_id;
	__u8	source_bus_irq;
	__u8	destination_ioapic_id;
	__u8	destination_ioapic_intin;
} __attribute__((packed));

struct k_mp_local_interrupt_entry {
	__u8	entry_type;
	__u8	interrupt_type;
	__u8	local_interrupt_flags;
	__u8	source_bus_id;
	__u8	source_bus_irq;
	__u8	destination_lapic_id;
	__u8	destination_lapic_intin;
} __attribute__((packed));

struct k_mp_info {
	int found;

	unsigned long ioapic_address;
};

extern struct k_mp_info k_mp;

void k_mp_get_info(void);

#endif

