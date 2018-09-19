#ifndef K_UEFI_H
#define K_UEFI_H

#include "kernel/include/types.h"

typedef u8		k_efi_uint8_t;
typedef u16		k_efi_uint16_t;
typedef u32		k_efi_uint32_t;
typedef u64		k_efi_uint64_t;
typedef unsigned long	k_efi_uintn_t;

typedef k_efi_uint64_t	k_efi_physical_address_t, k_efi_virtual_address_t;

struct efi_memory_descriptor {
	k_efi_uint32_t type;
	k_efi_physical_address_t physical_start;
	k_efi_virtual_address_t virtual_start;
	k_efi_uint64_t number_of_pages;
	k_efi_uint64_t attribute;
} __attribute__((packed));

#endif

