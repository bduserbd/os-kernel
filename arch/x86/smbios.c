#include "include/smbios.h"
#include "include/video.h"
#include "kernel/include/string.h"

static k_error_t k_smbios_checksum(void *ptr, int length)
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

static void *k_smbios_scan_address_range(k_uint32_t start, k_uint32_t range)
{
	k_uint8_t *ptr, *end;

	/* if (start & (0x10 - 1))
		   return 0; */

	ptr = (k_uint8_t *)start;
	end = ptr + range;

	while (ptr < end) {
		if (!k_memcmp(ptr, K_SMBIOS_SIGNATURE, sizeof(K_SMBIOS_SIGNATURE) - 1))
			return ptr;

		ptr += 0x10;
	}

	return NULL;
}

static struct k_smbios_entry_point *k_smbios_get_entry_point(void)
{
	k_error_t error;
	struct k_smbios_entry_point *ptr;

	ptr = k_smbios_scan_address_range(0xf0000, 0x10000);
	if (!ptr)
		return NULL;

	if (ptr->length != 0x1f)
		return NULL;

	error = k_smbios_checksum(ptr, ptr->length);
	if (error)
		return NULL;

	error = k_smbios_checksum((k_uint8_t *)ptr + 0x10, 0xf);
	if (error)
		return NULL;

	return ptr;
}

void k_smbios_get_info(void)
{
	struct k_smbios_entry_point *ptr;

	ptr = k_smbios_get_entry_point();
	if (!ptr)
		return;
}

