#ifndef K_ACPI_H
#define K_ACPI_H

#include "include/error.h"
#include "include/types.h"

#define K_ACPI_RSDP_SIGNATURE	"RSD PTR "

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

#define K_ACPI_RSDT_SIGNATURE	"RSDT"

struct k_acpi_rsdt {
	struct k_acpi_sdt sdt;
	__u32	entries[0];
} __attribute__((packed));

struct k_acpi_dsdt {
	struct k_acpi_sdt	sdt;
	__u8	aml[0];
} __attribute__((packed));
#endif

void k_acpi_get_info(void);

#endif

