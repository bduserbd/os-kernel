#include "include/init.h"
#include "include/video.h"
#include "include/idt.h"
#include "include/8259a.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/grub/multiboot.h"
#include "kernel/include/fb/shell.h"
#include "kernel/include/string.h"
#include "kernel/include/video/print.h"

extern __u8 __k_start[];
extern __u8 __k_end[];

void k_paging_init(void);
void k_paging_table_set_start(k_uint32_t);
void k_paging_reserve_pages(k_uint32_t, k_uint32_t);

static k_error_t k_reserve_reserved_pages(k_uint32_t ebx)
{
	k_uint32_t i;
	struct k_multiboot_info *mbi;
	struct k_multiboot_mmap_entry *entry;

	mbi = (void *)ebx;

	if ((mbi->flags & K_MULTIBOOT_INFO_MEM_MAP) == 0)
		return K_ERROR_NOT_FOUND;

	entry = (void *)mbi->mmap_addr;

	for (i = 0; i < mbi->mmap_length / sizeof(struct k_multiboot_mmap_entry); i++)
		if (entry[i].type == K_MULTIBOOT_MEMORY_RESERVED ||
				entry[i].type == K_MULTIBOOT_MEMORY_ACPI_RECLAIMABLE)
			if (entry[i].addr + entry[i].len > (1 << 20))
				k_paging_reserve_pages(entry[i].addr & 0xffffffff, entry[i].len & 0xffffffff);

	return K_ERROR_NONE;
}

k_error_t k_get_initramfs(k_uint32_t ebx, k_uint32_t *initramfs_start, k_uint32_t *initramfs_length)
{
	k_uint32_t i;
	struct k_multiboot_info *mbi;
	struct k_multiboot_mod_list *mod;

	mbi = (void *)ebx;

	*initramfs_start = *initramfs_length = 0x0;

	if ((mbi->flags & K_MULTIBOOT_INFO_MODS) == 0)
		return K_ERROR_NOT_FOUND;

	mod = (void *)mbi->mods_addr;

#define INITRAMS	"initramfs.img"

	for (i = 0; i < mbi->mods_count; i++)
		if (!k_strncmp((void *)mod[i].cmdline, INITRAMS, sizeof(INITRAMS) - 1)) {
			*initramfs_start = mod[i].mod_start;
			*initramfs_length = mod[i].mod_end - *initramfs_start;

			return K_ERROR_NONE;
		}

	return K_ERROR_NOT_FOUND;
}

k_error_t k_get_fb_info(k_uint32_t ebx, struct k_fb_info *fb)
{
	struct k_multiboot_info *mbi;

	mbi = (void *)ebx;

	if ((mbi->flags & K_MULTIBOOT_INFO_FRAMEBUFFER_INFO) == 0)
		return K_ERROR_NOT_FOUND;

	switch (mbi->framebuffer_type) {
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

void k_print_set_output_callback(void (*)(const char *));

void k_main(k_uint32_t eax, k_uint32_t ebx)
{
	k_error_t error;
	k_uint32_t page_table, heap;
	k_uint32_t initramfs_start, initramfs_length;
	struct k_fb_info fb;

	if (eax != K_MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	error = k_get_fb_info(ebx, &fb);
	if (error)
		return;

	k_text_set_info(&fb);
	k_print_set_output_callback(k_text_puts);

	k_pic_init();
	k_idt_init();

	page_table = K_ALIGN_UP(K_MAX((k_uint32_t)__k_end, ebx + *(k_uint32_t *)ebx), 0x1000);
	error = k_get_initramfs(ebx, &initramfs_start, &initramfs_length);
	if (error)
		return;

	page_table = K_ALIGN_UP(K_MAX(page_table, initramfs_start + initramfs_length), 0x1000);
	k_paging_table_set_start(page_table);

	k_paging_reserve_pages(0x0, 1 << 20);
	k_paging_reserve_pages((k_uint32_t)__k_start, __k_end - __k_start);
	k_paging_reserve_pages(ebx, *(k_uint32_t *)ebx);
	k_paging_reserve_pages(initramfs_start, initramfs_length);
	error = k_reserve_reserved_pages(ebx);
	if (error)
		return;

	// QEMU doesn't report APIC BIOS e820 memory map.
	k_paging_reserve_pages(0xfee00000, 0x1000);

	k_paging_init();

	heap = page_table + 0x1000 + 0x400 * 0x1000;
	k_buddy_init(heap);

	k_x86_init(NULL, NULL, initramfs_start, initramfs_length);
}

