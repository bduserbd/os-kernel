#include "include/mp.h"

static k_error_t k_mp_checksum(volatile void *ptr, int length)
{
	int i;
	k_uint8_t sum;

	sum = 0;

	for (i = 0; i < length; i++)
		sum += *((k_uint8_t *)ptr + i);

	if (!sum)
		return K_ERROR_NONE;
	else
		return K_ERROR_BAD_CHECKSUM;
}

static k_uint32_t k_mp_scan_address_range(k_uint32_t start, k_uint32_t range)
{
	k_uint32_t end;

	/* if (start & (0x10 - 1))
		   return 0; */

	end = start + range;

	while (start < end) {
		if (*(k_uint32_t *)start == K_SMP_SIGNATURE)
			return start;

		start += 0x10;
	}

	return 0;
}

static volatile struct k_mp_floating_pointer *k_mp_get_floating_pointer(void)
{
	k_error_t error;
	k_uint16_t ebda;
	volatile struct k_mp_floating_pointer *ptr;

	ptr = NULL;

	ebda = *(k_uint16_t *)0x40e;
	if (ebda)
		ptr = (void *)k_mp_scan_address_range(ebda << 4, 0x400);

	if (!ptr)
		(void)((ptr = (void *)k_mp_scan_address_range(639 * 0x400, 0x400)) ||
			(ptr = (void *)k_mp_scan_address_range(0xf0000, 0x10000)));

	if (!ptr)
		return NULL;

	if (ptr->configuration_type)
		return NULL;

	error = k_mp_checksum(ptr, ptr->length * 0x10);
	if (error)
		return NULL;

	return ptr;
}

void k_mp_get_info(void)
{
	k_uint16_t i;
	volatile struct k_mp_floating_pointer *ptr;
	volatile struct k_mp_configuration_table *config;
	k_uint8_t *entry_type;

	ptr = k_mp_get_floating_pointer();
	if (!ptr)
		return;

	config = (void *)ptr->configuration_table_pointer;
	if (config->signature != K_SMP_CONFIGURATION_TABLE_SIGNATURE)
		return;

	entry_type = (k_uint8_t *)config + sizeof(struct k_mp_configuration_table);
	for (i = 0; i < config->entry_count; i++) {
		if (*entry_type == K_SMP_ENTRY_PROCESSOR) {
			entry_type += 20;
		} else {
			entry_type += 8;
		}
	}
}

