#ifndef K_SMBIOS_H
#define K_SMBIOS_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

/* SMBIOS 2.1 */
#define K_SMBIOS_SIGNATURE	"_SM_"

struct k_smbios_entry_point {
	__u8	signature[4];
	__u8	checksum;
	__u8	length;
	__u8	major_version;
	__u8	minor_version;
	__u16	max_structure_size;
	__u8	revision;
	__u8	formatted_area[5];
	__u8	signature2[5];
	__u8	checksum2;
	__u16	table_length;
	__u32	table_address;
	__u16	structures;
	__u8	bcd_revision;
} __attribute__((packed));

/* SMBIOS structure header format. */
#define K_SMBIOS_STRUCTURE_BIOS				0
#define K_SMBIOS_STRUCTURE_SYSTEM			1
#define K_SMBIOS_STRUCTURE_BASEBOARD			2
#define K_SMBIOS_STRUCTURE_CHASSIS			3
#define K_SMBIOS_STRUCTURE_PROCESSOR			4
#define K_SMBIOS_STRUCTURE_CACHE			7
#define K_SMBIOS_STRUCTURE_SYSTEM_SLOTS			9
#define K_SMBIOS_STRUCTURE_PHYSICAL_MEMORY_ARRAY	16
#define K_SMBIOS_STRUCTURE_MEMORY_DEVICE		17
#define K_SMBIOS_STRUCTURE_MEMORY_ARRAY_MAPPED_ADDRESS	19
#define K_SMBIOS_STRUCTURE_SYSTEM_BOOT			32

struct k_smbios_structure_header {
	__u8	type;
	__u8	length;
	__u16	handle;
} __attribute__((packed));

/* SMBIOS BIOS information. */
struct k_smbios_bios {
	__u8	type;
	__u8	length;
	__u16	handle;
	__u8	vendor;
	__u8	version;
	__u16	starting_address_segment;
	__u8	release_date;
	__u8	rom_size;
	__u64	characteristics;
	__u16	characteristics2;
	__u8	major_release;
	__u8	minor_release;
	__u8	firmware_major_release;
	__u8	firmware_minor_release;
} __attribute__((packed));

/* SMBIOS system information. */
struct k_smbios_system {
	__u8	type;
	__u8	length;
	__u16	handle;
	__u8	manufacturer;
	__u8	product_name;
	__u8	version;
	__u8	serial_number;
	__u8	uuid[16];
	__u8	wake_up;
	__u8	sku_number;
	__u8	family;
} __attribute__((packed));

/* SMBIOS chassis. */
struct k_smbios_chassis {
	__u8	type;
	__u8	length;
	__u16	handle;
	__u8	manufacturer;
	__u8	enclosure;
	__u8	version;
	__u8	serial_number;
	__u8	asset_tag_number;
	__u8	boot_up_state;
	__u8	power_supply_state;
	__u8	thermal_state;
	__u8	security_status;
	__u32	oem;
	__u8	height;
	__u8	power_cords;
	__u8	element_count;
	__u8	element_record_length;
} __attribute__((packed));

void k_smbios_get_info(void);

#endif

