#include "include/init.h"
#include "include/paging.h"
#include "include/video.h"
#include "include/idt.h"
#include "include/8259a.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/mm/zone.h"
#include "kernel/include/grub/multiboot.h"
#include "kernel/include/fb/shell.h"
#include "kernel/include/string.h"
#include "kernel/include/video/print.h"

extern __u8 __k_start[];
extern __u8 __k_end[];

unsigned long k_multiboot_magic_ptr;
unsigned long k_multiboot_info_ptr;

unsigned long k_initramfs_start;
unsigned long k_initramfs_length;

/* Functions from here are called when high virtual memory is initialized. */
k_error_t k_reserve_reserved_pages(void)
{
	k_uint32_t i;
	struct k_multiboot_info *mbi;
	struct k_multiboot_mmap_entry *entry;

	mbi = (void *)k_multiboot_info_ptr;

	if ((mbi->flags & K_MULTIBOOT_INFO_MEM_MAP) == 0)
		return K_ERROR_NOT_FOUND;

	// TODO: What if we wiped memory map descriptors ?
	entry = (void *)k_p2v_l(mbi->mmap_addr);

	for (i = 0; i < mbi->mmap_length / sizeof(struct k_multiboot_mmap_entry); i++)
		if (entry[i].type == K_MULTIBOOT_MEMORY_RESERVED ||
				entry[i].type == K_MULTIBOOT_MEMORY_ACPI_RECLAIMABLE)
			if (entry[i].addr + entry[i].len > K_MB(1)) {
				k_printf("%x %x\n", (k_uint32_t)entry[i].addr >> 12,
						(k_uint32_t)entry[i].len >> 12);
				k_memory_zone_dma_add(entry[i].addr >> 12, entry[i].len >> 12);
			}

	// QEMU doesn't report APIC BIOS e820 memory map.
	k_memory_zone_dma_add(0xfee00000 >> 12, 1);

	return K_ERROR_NONE;
}

k_error_t k_get_fb_info(struct k_multiboot_info *mbi, struct k_fb_info *fb)
{
	if ((mbi->flags & K_MULTIBOOT_INFO_FRAMEBUFFER_INFO) == 0)
		return K_ERROR_NOT_FOUND;

	switch (mbi->framebuffer_type) {
		case K_MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			fb->width = mbi->framebuffer_width;
			fb->height = mbi->framebuffer_height;

			fb->framebuffer = K_VIRTUAL_ADDRESS(mbi->framebuffer_addr);

			break;

		default:
			return K_ERROR_NOT_FOUND;
	}

	return K_ERROR_NONE;
}

void k_print_set_output_callback(void (*)(const char *));
void k_paging_arch_init(void);

k_error_t k_main(void)
{
	k_error_t error;
	struct k_multiboot_info *mbi;
	struct k_fb_info fb;

	k_multiboot_info_ptr += K_IMAGE_BASE;
	k_initramfs_start += K_IMAGE_BASE;

	k_paging_arch_init();

	mbi = (void *)k_multiboot_info_ptr;
	k_paging_reserve_pages((unsigned long)mbi, sizeof(struct k_multiboot_info));

	error = k_get_fb_info(mbi, &fb);
	if (error)
		return error;

	k_paging_reserve_pages(K_VIRTUAL_ADDRESS(0x0), K_MB(1));

	k_text_set_info(&fb);
	k_print_set_output_callback(k_text_puts);

	k_pic_init();
	k_idt_init();

	*(k_uint8_t *)0xa000000 = 'A';

	k_paging_build_frame_array(K_PAGE_NUMBER(K_KB(mbi->mem_upper)));
	k_buddy_init(K_ALIGN_UP(K_OFFSET_FROM(k_normal_frames, K_FRAME_ARRAY_SIZE), 0x1000));

	k_paging_reserve_pages(k_initramfs_start, k_initramfs_length);

	k_x86_init(NULL, NULL, k_initramfs_start, k_initramfs_length);

	return K_ERROR_FAILURE;
}

