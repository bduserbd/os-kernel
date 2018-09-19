#ifndef K_MULTIBOOT2_H
#define K_MULTIBOOT2_H

#ifndef __ASSEMBLER__
#include "kernel/include/types.h"
#endif

#define K_MULTIBOOT2_HEADER_MAGIC	0xe85250d6

#define K_MULTIBOOT2_BOOTLOADER_MAGIC	0x36d76289

#define K_MULTIBOOT2_TAG_TYPE_END		0

#define K_MULTIBOOT2_TAG_TYPE_MMAP		6
#ifdef K_CONFIG_UEFI
#define K_MULTIBOOT2_TAG_TYPE_FRAMEBUFFER	8
#define K_MULTIBOOT2_TAG_TYPE_EFI_MMAP		17
#endif

#define K_MULTIBOOT2_HEADER_TAG_END		0
#define K_MULTIBOOT2_HEADER_TAG_FRAMEBUFFER	5

#define K_MULTIBOOT2_ARCHITECTURE_I386		0
#define K_MULTIBOOT2_ARCHITECTURE_MIPS32	4

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

#ifdef K_CONFIG_UEFI

#define K_MULTIBOOT2_FRAMEBUFFER_TYPE_INDEXED	0
#define K_MULTIBOOT2_FRAMEBUFFER_TYPE_RGB	1
#define K_MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT	2

struct k_multiboot2_tag_framebuffer {
	k_uint32_t type;
	k_uint32_t size;
	k_uint64_t framebuffer_addr;
	k_uint32_t framebuffer_pitch;
	k_uint32_t framebuffer_width;
	k_uint32_t framebuffer_height;
	k_uint8_t framebuffer_bpp;
	k_uint8_t framebuffer_type;
	k_uint8_t reserved;
	k_uint8_t framebuffer_red_field_position;
	k_uint8_t framebuffer_red_mask_size;
	k_uint8_t framebuffer_green_field_position;
	k_uint8_t framebuffer_green_mask_size;
	k_uint8_t framebuffer_blue_field_position;
	k_uint8_t framebuffer_blue_mask_size;
} __attribute__((packed));

struct k_multiboot2_tag_efi_mmap {
	k_uint32_t type;
	k_uint32_t size;
	k_uint32_t descr_size;
	k_uint32_t descr_vers;
} __attribute__((packed));

#endif

#endif

#endif

