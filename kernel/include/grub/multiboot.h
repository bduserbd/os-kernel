#ifndef K_MULTIBOOT_H
#define K_MULTIBOOT_H

#ifndef __ASSEMBLER__
#include "kernel/include/types.h"
#endif

#define K_MULTIBOOT_HEADER_MAGIC	0x1badb002

#define K_MULTIBOOT_BOOTLOADER_MAGIC	0x2badb002

#define K_MULTIBOOT_INFO_MEMORY			0x00000001
#define K_MULTIBOOT_INFO_BOOTDEV		0x00000002
#define K_MULTIBOOT_INFO_CMDLINE		0x00000004
#define K_MULTIBOOT_INFO_MODS			0x00000008
#define K_MULTIBOOT_INFO_AOUT_SYMS		0x00000010
#define K_MULTIBOOT_INFO_ELF_SHDR		0x00000020
#define K_MULTIBOOT_INFO_MEM_MAP		0x00000040
#define K_MULTIBOOT_INFO_DRIVE_INFO		0x00000080
#define K_MULTIBOOT_INFO_CONFIG_TABLE		0x00000100
#define K_MULTIBOOT_INFO_BOOT_LOADER_NAME	0x00000200
#define K_MULTIBOOT_INFO_APM_TABLE		0x00000400
#define K_MULTIBOOT_INFO_VBE_INFO		0x00000800
#define K_MULTIBOOT_INFO_FRAMEBUFFER_INFO	0x00001000

#ifndef __ASSEMBLER__

struct k_multiboot_aout_symbol_table {
	k_uint32_t tabsize;
	k_uint32_t strsize;
	k_uint32_t addr;
	k_uint32_t reserved;
} __attribute__((packed));

struct k_multiboot_elf_section_header_table {
	k_uint32_t num;
	k_uint32_t size;
	k_uint32_t addr;
	k_uint32_t shndx;
} __attribute__((packed));

struct k_multiboot_mod_list {
	k_uint32_t mod_start;
	k_uint32_t mod_end;
	k_uint32_t cmdline;
	k_uint32_t pad;
} __attribute__((packed));

struct k_multiboot_info {
	k_uint32_t flags;

	k_uint32_t mem_lower;
	k_uint32_t mem_upper;

	k_uint32_t boot_device;

	k_uint32_t cmdline;

	k_uint32_t mods_count;
	k_uint32_t mods_addr;

	union {
		struct k_multiboot_aout_symbol_table aout_sym;
		struct k_multiboot_elf_section_header_table elf_sec;
	} u;

	k_uint32_t mmap_length;
	k_uint32_t mmap_addr;

	k_uint32_t drives_length;
	k_uint32_t drives_addr;

	k_uint32_t config_table;

	k_uint32_t boot_loader_name;

	k_uint32_t apm_table;

	k_uint32_t vbe_control_info;
	k_uint32_t vbe_mode_info;
	k_uint16_t vbe_mode;
	k_uint16_t vbe_interface_seg;
	k_uint16_t vbe_interface_off;
	k_uint16_t vbe_interface_len;

	k_uint64_t framebuffer_addr;
	k_uint32_t framebuffer_pitch;
	k_uint32_t framebuffer_width;
	k_uint32_t framebuffer_height;
	k_uint8_t framebuffer_bpp;
	k_uint8_t framebuffer_type;

	k_uint8_t framebuffer_red_field_position;
	k_uint8_t framebuffer_red_mask_size;
	k_uint8_t framebuffer_green_field_position;
	k_uint8_t framebuffer_green_mask_size;
	k_uint8_t framebuffer_blue_field_position;
	k_uint8_t framebuffer_blue_mask_size;
} __attribute__((packed));

#endif

#endif

