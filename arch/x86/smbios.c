#include "include/smbios.h"
#include "include/video.h"
#include "kernel/include/string.h"

static struct k_smbios_info {
	k_uint8_t major_version;
	k_uint8_t minor_version;

	struct {
		const char *vendor, *version, *release_date;
	} bios;

	struct {
		const char *manufacturer, *product_name, *version, *serial_number;
	} system;

	struct {
		const char *manufacturer, *version, *serial_number;
	} chassis;
} k_smbios;

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

	k_smbios.major_version = ptr->major_version;
	k_smbios.minor_version = ptr->minor_version;

	return ptr;
}

static int k_smbios_strings_length(const char *strings)
{
	const char *end;

	for (end = strings; *(k_uint16_t *)end; end++) ;

	return 2 + end - strings;
}

static const char *k_smbios_get_string(const char *strings, int index)
{
	if (!index)
		return NULL;

	index--;

	while (index > 0) {
		if (!*strings)
			index--;

		strings++;
	}

	return strings;
}

static int k_smbios_get_next_structure(struct k_smbios_structure_header *header)
{
	return header->length + k_smbios_strings_length((char *)header + header->length);
}

static int k_smbios_parse_chassis_information(struct k_smbios_chassis *chassis)
{
	const char *strings;

	strings = (char *)chassis + chassis->length;

	if (chassis->manufacturer)
		k_smbios.chassis.manufacturer = k_smbios_get_string(strings, chassis->manufacturer);

	if (chassis->version)
		k_smbios.chassis.version = k_smbios_get_string(strings, chassis->version);

	if (chassis->serial_number)
		k_smbios.chassis.serial_number = k_smbios_get_string(strings, chassis->serial_number);

	return chassis->length + k_smbios_strings_length(strings);
}

static int k_smbios_parse_system_information(struct k_smbios_system *system)
{
	const char *strings;

	strings = (char *)system + system->length;

	if (system->manufacturer)
		k_smbios.system.manufacturer = k_smbios_get_string(strings, system->manufacturer);

	if (system->product_name)
		k_smbios.system.product_name = k_smbios_get_string(strings, system->product_name);

	if (system->version)
		k_smbios.system.version = k_smbios_get_string(strings, system->version);

	if (system->serial_number)
		k_smbios.system.serial_number = k_smbios_get_string(strings, system->serial_number);

	return system->length + k_smbios_strings_length(strings);
}

static int k_smbios_parse_bios_information(struct k_smbios_bios *bios)
{
	const char *strings;

	strings = (char *)bios + bios->length;

	if (bios->vendor)
		k_smbios.bios.vendor = k_smbios_get_string(strings, bios->vendor);

	if (bios->version)
		k_smbios.bios.version = k_smbios_get_string(strings, bios->version);

	if (bios->release_date)
		k_smbios.bios.release_date = k_smbios_get_string(strings, bios->release_date);

	return bios->length + k_smbios_strings_length(strings);
}

void k_smbios_get_info(void)
{
	int length;
	k_uint16_t i;
	k_uint8_t *header;
	struct k_smbios_entry_point *ptr;

	ptr = k_smbios_get_entry_point();
	if (!ptr)
		return;

	if (k_smbios.major_version < 2)
		return;

	header = (k_uint8_t *)ptr->table_address;
	for (i = 0; i < ptr->structures; i++) {
		switch (*header) {
		case K_SMBIOS_STRUCTURE_BIOS:
			length = k_smbios_parse_bios_information((void *)header);
			break;

		case K_SMBIOS_STRUCTURE_SYSTEM:
			length = k_smbios_parse_system_information((void *)header);
			break;

		case K_SMBIOS_STRUCTURE_CHASSIS:
			length = k_smbios_parse_chassis_information((void *)header);
			break;

		default:
			length = k_smbios_get_next_structure((void *)header);
			break;
		}

		header += length;
	}
}

