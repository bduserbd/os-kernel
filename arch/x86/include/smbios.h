#ifndef K_SMBIOS_H
#define K_SMBIOS_H

#include "include/types.h"
#include "include/error.h"

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
	__u8	formatted_area;
	__u8	signature2[5];
	__u8	checksum2;
	__u16	table_length;
	__u32	table_address;
	__u16	entries;
	__u8	bcd_revision;
} __attribute__((packed));

void k_smbios_get_info(void);

#endif

