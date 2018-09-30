#include "include/init.h"
#include "include/idt.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/grub/multiboot.h"
#include "kernel/include/fb/shell.h"
#include "kernel/include/string.h"

extern __u8 __k_start[];
extern __u8 __k_end[];

void k_paging_init(void);
void k_paging_table_set_start(k_uint32_t);
void k_paging_reserve_pages(k_uint32_t, k_uint32_t);

static k_error_t k_reserve_reserved_pages(k_uint32_t mmap_addr, k_uint32_t mmap_length)
{
	k_uint32_t i;
	struct k_multiboot_mmap_entry *entry;

	entry = (void *)mmap_addr;

	for (i = 0; i < mmap_length / sizeof(struct k_multiboot_mmap_entry); i++)
		if (entry[i].type == K_MULTIBOOT_MEMORY_RESERVED ||
				entry[i].type == K_MULTIBOOT_MEMORY_ACPI_RECLAIMABLE)
			if (entry[i].addr + entry[i].len > (1 << 20))
				k_paging_reserve_pages(entry[i].addr & 0xffffffff, entry[i].len & 0xffffffff);

	return K_ERROR_NONE;
}

k_error_t k_get_initramfs(k_uint32_t mods_addr, k_uint32_t mods_count,
		k_uint32_t *initramfs_start, k_uint32_t *initramfs_end)
{
	k_uint32_t i;
	struct k_multiboot_mod_list *mod;

	*initramfs_start = *initramfs_end = 0x0;

	mod = (void *)mods_addr;

#define INITRAMS	"initramfs.img"

	for (i = 0; i < mods_count; i++)
		if (!k_strncmp((void *)mod[i].cmdline, INITRAMS, sizeof(INITRAMS) - 1)) {
			*initramfs_start = mod[i].mod_start;
			*initramfs_end = mod[i].mod_end;

			k_paging_reserve_pages(*initramfs_start, *initramfs_end - *initramfs_start);

			return K_ERROR_NONE;
		}

	return K_ERROR_NOT_FOUND;
}

k_error_t k_get_fb_info(struct k_multiboot_info *mbi, struct k_fb_info *fb)
{
	switch (mbi->framebuffer_type) {
	case K_MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
		fb->width = mbi->framebuffer_width;
		fb->height = mbi->framebuffer_height;

		fb->pitch = mbi->framebuffer_pitch;

		fb->framebuffer = mbi->framebuffer_addr;

		fb->bits_per_pixel = mbi->framebuffer_bpp;
		fb->bytes_per_pixel = fb->bits_per_pixel >> 3;

		fb->red.position = mbi->framebuffer_red_field_position;
		fb->red.mask = (1 << mbi->framebuffer_red_mask_size) - 1;

		fb->green.position = mbi->framebuffer_green_field_position;
		fb->green.mask = (1 << mbi->framebuffer_green_mask_size) - 1;

		fb->blue.position = mbi->framebuffer_blue_field_position;
		fb->blue.mask = (1 << mbi->framebuffer_blue_mask_size) - 1;

		fb->reserved.position = 24;
		fb->reserved.mask = 0xff;

		k_paging_reserve_pages(fb->framebuffer, fb->height * fb->pitch);

	case K_MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
		fb->width = mbi->framebuffer_width;
		fb->height = mbi->framebuffer_height;

		fb->framebuffer = mbi->framebuffer_addr;

		break;

	default:
		return K_ERROR_NOT_FOUND;
	}

	return K_ERROR_NONE;
}

k_error_t k_scan_multiboot_tags(k_uint32_t ebx, struct k_fb_info *fb,
		k_uint32_t *initramfs_start, k_uint32_t *initramfs_end)
{
	k_error_t error;
	struct k_multiboot_info *mbi;

	mbi = (void *)ebx;

	if (mbi->flags & K_MULTIBOOT_INFO_MEM_MAP)
		k_reserve_reserved_pages(mbi->mmap_addr, mbi->mmap_length);
	else
		return K_ERROR_NOT_FOUND;

	if (mbi->flags & K_MULTIBOOT_INFO_MODS) {
		error = k_get_initramfs(mbi->mods_addr, mbi->mods_count,
				initramfs_start, initramfs_end);
		if (error)
			return error;
	}

	if (mbi->flags & K_MULTIBOOT_INFO_FRAMEBUFFER_INFO)
		k_get_fb_info(mbi, fb);
	else
		return K_ERROR_NOT_FOUND;

	return K_ERROR_NONE;
}

void k_main(k_uint32_t eax, k_uint32_t ebx)
{
	k_error_t error;
	k_uint32_t page_table, heap;
	k_uint32_t initramfs_start, initramfs_end;
	struct k_fb_info fb;

	if (eax != K_MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	k_idt_init();

	page_table = K_ALIGN_UP(K_MAX((k_uint32_t)__k_end, ebx + *(k_uint32_t *)ebx), 0x1000);
	k_paging_table_set_start(page_table);

	k_paging_reserve_pages(0x0, 1 << 20);
	k_paging_reserve_pages((k_uint32_t)__k_start, __k_end - __k_start);
	k_paging_reserve_pages(ebx, *(k_uint32_t *)ebx);

	error = k_scan_multiboot_tags(ebx, &fb, &initramfs_start, &initramfs_end);
	if (error)
		return;

	// QEMU doesn't report APIC BIOS e820 memory map.
	k_paging_reserve_pages(0xfee00000, 0x1000);

	k_paging_init();

	heap = page_table + 0x1000 + 0x400 * 0x1000;

	k_x86_init(heap, &fb, NULL);
}

