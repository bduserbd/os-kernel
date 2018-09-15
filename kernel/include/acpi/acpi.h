#ifndef K_ACPI_H
#define K_ACPI_H

#include "acpi-tables.h"

struct k_acpi_info {
	int found;

	k_uint32_t lapic_address;
};

extern struct k_acpi_info k_acpi;

void k_acpi_get_info(void);

#endif

