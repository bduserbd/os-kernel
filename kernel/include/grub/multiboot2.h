#ifndef K_MULTIBOOT2_H
#define K_MULTIBOOT2_H

#ifndef __ASSEMBLER__
#include "kernel/include/types.h"
#include "kernel/include/acpi/acpi.h"

#ifdef K_CONFIG_UEFI
#include "kernel/include/uefi/uefi.h"
#endif

#endif

#define K_MULTIBOOT2_HEADER_MAGIC	0xe85250d6

#define K_MULTIBOOT2_BOOTLOADER_MAGIC	0x36d76289

#define K_MULTIBOOT2_TAG_TYPE_END		0
#define K_MULTIBOOT2_TAG_TYPE_MODULE		3
#define K_MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO	4
#define K_MULTIBOOT2_TAG_TYPE_MMAP		6
#define K_MULTIBOOT2_TAG_TYPE_FRAMEBUFFER	8
#define K_MULTIBOOT2_TAG_TYPE_SMBIOS		13
#define K_MULTIBOOT2_TAG_TYPE_ACPI_OLD		14
#define K_MULTIBOOT2_TAG_TYPE_ACPI_NEW		15
#ifdef K_CONFIG_UEFI
#define K_MULTIBOOT2_TAG_TYPE_EFI_MMAP		17
#endif

#define K_MULTIBOOT2_HEADER_TAG_END		0
#define K_MULTIBOOT2_HEADER_TAG_FRAMEBUFFER	5

#define K_MULTIBOOT2_ARCHITECTURE_I386		0
#define K_MULTIBOOT2_ARCHITECTURE_MIPS32	4

#define K_MULTIBOOT2_FRAMEBUFFER_TYPE_INDEXED	0
#define K_MULTIBOOT2_FRAMEBUFFER_TYPE_RGB	1
#define K_MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT	2

#define K_MULTIBOOT2_MEMORY_AVAILABLE		1
#define K_MULTIBOOT2_MEMORY_RESERVED		2
#define K_MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE	3
#define K_MULTIBOOT2_MEMORY_NVS			4
#define K_MULTIBOOT2_MEMORY_BADRAM		5

#ifndef __ASSEMBLER__

struct k_multiboot2_tag {
	k_uint32_t type;
	k_uint32_t size;
} __attribute__((packed));

struct k_multiboot2_tag_module {
	k_uint32_t type;
	k_uint32_t size;
	k_uint32_t mod_start;
	k_uint32_t mod_end;
	char cmdline[0];
} __attribute__((packed));

struct k_multiboot2_tag_basic_meminfo {
	k_uint32_t type;
	k_uint32_t size;
	k_uint32_t mem_lower;
	k_uint32_t mem_upper;
} __attribute__((packed));

struct k_multiboot2_mmap_entry {
	k_uint64_t addr;
	k_uint64_t len;
	k_uint32_t type;
	k_uint32_t zero;
} __attribute__((packed));

struct k_multiboot2_tag_mmap {
	k_uint32_t type;
	k_uint32_t size;
	k_uint32_t entry_size;
	k_uint32_t entry_version;
	struct k_multiboot2_mmap_entry entries[0];
} __attribute__((packed));

struct k_multiboot2_tag_framebuffer {
	k_uint32_t type;
	k_uint32_t size;
	k_uint64_t addr;
	k_uint32_t pitch;
	k_uint32_t width;
	k_uint32_t height;
	k_uint8_t bpp;
	k_uint8_t framebuffer_type;
	k_uint16_t reserved;
	k_uint8_t red_field_position;
	k_uint8_t red_mask_size;
	k_uint8_t green_field_position;
	k_uint8_t green_mask_size;
	k_uint8_t blue_field_position;
	k_uint8_t blue_mask_size;
} __attribute__((packed));

struct k_multiboot2_tag_smbios {
	k_uint32_t type;
	k_uint32_t size;
	k_uint8_t major;
	k_uint8_t minor;
	k_uint8_t reserved[6];
	k_uint8_t tables[0];
} __attribute__((packed));

struct k_multiboot2_tag_old_acpi {
	k_uint32_t type;
	k_uint32_t size;
	k_uint8_t rsdp[0];
} __attribute__((packed));

struct k_multiboot2_tag_new_acpi {
	k_uint32_t type;
	k_uint32_t size;
	k_uint8_t rsdp[0];
} __attribute__((packed));

struct k_multiboot2_tag_efi_mmap {
	k_uint32_t type;
	k_uint32_t size;
	k_uint32_t descr_size;
	k_uint32_t descr_vers;
	struct k_efi_memory_descriptor efi_mmap[0];
} __attribute__((packed));

#endif

#endif

