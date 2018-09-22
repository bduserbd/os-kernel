#ifndef K_UEFI_H
#define K_UEFI_H

#include "kernel/include/types.h"

typedef u8		k_efi_uint8_t;
typedef u16		k_efi_uint16_t;
typedef u32		k_efi_uint32_t;
typedef u64		k_efi_uint64_t;
typedef unsigned long	k_efi_uintn_t;

typedef k_efi_uint64_t	k_efi_physical_address_t, k_efi_virtual_address_t;

/* EFI Memory regions type. */
typedef enum {
	K_EFI_RESERVED_MEMORY_TYPE,
	K_EFI_LOADER_CODE,
	K_EFI_LOADER_DATA,
	K_EFI_BOOT_SERVICES_CODE,
	K_EFI_BOOT_SERVICES_DATA,
	K_EFI_RUNTIME_SERVICES_CODE,
	K_EFI_RUNTIME_SERVICES_DATA,
	K_EFI_CONVENTIONAMEMORY,
	K_EFI_UNUSABLE_MEMORY,
	K_EFI_ACPI_RECLAIM_MEMORY,
	K_EFI_ACPI_MEMORY_NVS,
	K_EFI_MEMORY_MAPPED_IO,
	K_EFI_MEMORY_MAPPED_IO_PORT_SPACE,
	K_EFI_PACODE,
	K_EFI_PERSISTENT_MEMORY,
	K_EFI_MAX_MEMORY_TYPE,
} k_efi_memory_type_t;

/* EFI Memory Attribute Definition. */
#define K_EFI_MEMORY_UC			0x0000000000000001
#define K_EFI_MEMORY_WC			0x0000000000000002
#define K_EFI_MEMORY_WT			0x0000000000000004
#define K_EFI_MEMORY_WB			0x0000000000000008
#define K_EFI_MEMORY_UCE		0x0000000000000010
#define K_EFI_MEMORY_WP			0x0000000000001000
#define K_EFI_MEMORY_RP			0x0000000000002000
#define K_EFI_MEMORY_XP			0x0000000000004000
#define K_EFI_MEMORY_NV			0x0000000000008000
#define K_EFI_MEMORY_MORE_RELIABLE	0x0000000000010000
#define K_EFI_MEMORY_RO			0x0000000000020000
#define K_EFI_MEMORY_RUNTIME		0x8000000000000000

/* EFI Memory Descriptor. */
struct k_efi_memory_descriptor {
	k_efi_uint32_t type;
	k_efi_physical_address_t physical_start;
	k_efi_virtual_address_t virtual_start;
	k_efi_uint64_t number_of_pages;
	k_efi_uint64_t attribute;
} __attribute__((packed));

#endif

