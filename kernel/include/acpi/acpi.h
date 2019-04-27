#ifndef K_ACPI_H
#define K_ACPI_H

#include "acpi-tables.h"

struct k_acpi_id {
	int valid;
	k_uint8_t processor;
	k_uint8_t lapic;
};

struct k_acpi_info {
	int found;

	unsigned long physical_rsdp;

	struct k_acpi_id ids[K_CONFIG_CPUS];

	int interrupts;

	struct {
		int source;
		int destination;
	} interrupt[16];

	unsigned long ioapic_address;

	unsigned long lapic_address;

	unsigned long hpet_address;
};

extern struct k_acpi_info k_acpi;

void k_acpi_get_info(void *);

#endif

