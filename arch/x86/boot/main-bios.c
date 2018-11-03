#include "include/init.h"
#include "include/paging.h"
#include "include/video.h"
#include "include/idt.h"
#include "include/8259a.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/grub/multiboot.h"
#include "kernel/include/fb/shell.h"
#include "kernel/include/string.h"
#include "kernel/include/video/print.h"

extern __u8 __k_start[];
extern __u8 __k_end[];

extern unsigned long *k_multiboot_magic_ptr;
extern unsigned long *k_multiboot_info_ptr;

static unsigned long k_initramfs_start = 0;
static unsigned long k_initramfs_length = 0;

/* Functions from here are called when high virtual memory isn't set. */
k_error_t k_get_initramfs(struct k_multiboot_info *mbi,
		k_uint32_t *initramfs_start, k_uint32_t *initramfs_length)
{
	k_uint32_t i;
	struct k_multiboot_mod_list *mod;

	*initramfs_start = *initramfs_length = 0x0;

	if ((mbi->flags & K_MULTIBOOT_INFO_MODS) == 0)
		return K_ERROR_NOT_FOUND;

	mod = (void *)mbi->mods_addr;

	for (i = 0; i < mbi->mods_count; i++) {
		char *s = (char *)mod[i].cmdline;

		if (s[0] == 'i' && s[1] == 'n' && s[2] == 'i' && s[3] == 't' &&
				s[4] == 'r' && s[5] == 'a' && s[6] == 'm' &&
				s[7] == 'f' && s[8] == 's' &&
				s[9] == '.' && s[10] == 'i' && s[11] == 'm' && s[12] == 'g') {
			*initramfs_start = mod[i].mod_start;
			*initramfs_length = mod[i].mod_end - *initramfs_start;

			return K_ERROR_NONE;
		}
	}

	return K_ERROR_NOT_FOUND;
}

k_error_t k_is_valid_multiboot(void)
{
	unsigned long magic;

	magic = K_VALUE_PHYSICAL_ADDRESS(&k_multiboot_magic_ptr);

	if (magic == K_MULTIBOOT_BOOTLOADER_MAGIC)
		return K_ERROR_NONE;
	else
		return K_ERROR_INVALID_PARAMETER;
}

k_uint32_t k_alloc_boot_page_table(void)
{
	k_error_t error;
	struct k_multiboot_info *mbi;
	k_uint32_t page_table;
	k_uint32_t initramfs_start, initramfs_length;

	mbi = (void *)K_VALUE_PHYSICAL_ADDRESS(&k_multiboot_info_ptr);

	page_table = K_ALIGN_UP(K_MAX(K_PHYSICAL_ADDRESS((k_uint32_t)__k_end),
				(unsigned long)mbi + sizeof(struct k_multiboot_info)), 0x1000);

	error = k_get_initramfs(mbi, &initramfs_start, &initramfs_length);
	if (error)
		return 0;

	K_VALUE_PHYSICAL_ADDRESS(&k_initramfs_start) = K_VIRTUAL_ADDRESS(initramfs_start);
	K_VALUE_PHYSICAL_ADDRESS(&k_initramfs_length) = initramfs_length;

	return K_ALIGN_UP(K_MAX(page_table, initramfs_start + initramfs_length), 0x1000);
}

/* Functions from here are called when high virtual memory is initialized. */
static k_error_t k_reserve_reserved_pages(struct k_multiboot_info *mbi)
{
	k_uint32_t i;
	struct k_multiboot_mmap_entry *entry;

	if ((mbi->flags & K_MULTIBOOT_INFO_MEM_MAP) == 0)
		return K_ERROR_NOT_FOUND;

	// TODO: What if we wiped these ?
	entry = (void *)k_p2v_l(mbi->mmap_addr);

	for (i = 0; i < mbi->mmap_length / sizeof(struct k_multiboot_mmap_entry); i++)
		if (entry[i].type == K_MULTIBOOT_MEMORY_RESERVED ||
				entry[i].type == K_MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
			if (entry[i].addr + entry[i].len > K_MB(1)) {
				k_printf("%x %x\n", (k_uint32_t)entry[i].addr, (k_uint32_t)entry[i].len);
				//k_paging_reserve_pages(K_VIRTUAL_ADDRESS(entry[i].addr & 0xffffffff),
				//		entry[i].len & 0xffffffff);
			}
		}

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

k_error_t k_main(k_uint32_t eax, k_uint32_t ebx)
{
	k_error_t error;
	struct k_multiboot_info *mbi;
	struct k_fb_info fb;

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

	k_paging_build_frame_array(K_KB(mbi->mem_upper) >> 12);

	k_buddy_init(K_ALIGN_UP(K_OFFSET_FROM(k_frames, K_FRAME_ARRAY_SIZE), 0x1000));

	k_reserve_reserved_pages(mbi);

	k_paging_reserve_pages(k_initramfs_start, k_initramfs_length);

#if 0
	// QEMU doesn't report APIC BIOS e820 memory map.
	k_paging_reserve_pages(0xfee00000, 0x1000);


	k_x86_init(NULL, NULL, k_initramfs_start, k_initramfs_length);
#endif

	return K_ERROR_FAILURE;
}

