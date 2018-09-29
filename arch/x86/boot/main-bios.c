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

#if 0
static k_error_t k_reserve_reserved_pages(void *tag, void *data)
{
	k_uint32_t i;
	struct k_multiboot2_tag_mmap *mmaptag;
	struct k_multiboot2_mmap_entry *entry;

	mmaptag = tag;

	if (mmaptag->entry_size != sizeof(*entry))
		return K_ERROR_NOT_FOUND;

	entry = &mmaptag->entries[0];
	for (i = 0; i < (mmaptag->size - sizeof(struct k_multiboot2_tag_mmap)) /
			mmaptag->entry_size; i++, entry++)
		if (entry->type == K_MULTIBOOT2_MEMORY_RESERVED ||
				entry->type == K_MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE)
			if (entry->addr + entry->len > (1 << 20))
				k_paging_reserve_pages(entry->addr & 0xffffffff, entry->len & 0xffffffff);

	return K_ERROR_NONE;
}

k_error_t k_get_initramfs(void *tag, void *data)
{
	struct k_multiboot2_tag_module *moduletag;

	moduletag = tag;

	if (!k_memcmp("initrd", moduletag->cmdline, 5))
		return K_ERROR_NONE;
	else
		return K_ERROR_NOT_FOUND;
}

k_error_t k_get_fb_info(void *tag, void *data)
{
	struct k_fb_info *fb;
	struct k_multiboot2_tag_framebuffer *fbtag;

	fbtag = tag;
	fb = data;

	switch (fbtag->framebuffer_type) {
	case K_MULTIBOOT2_FRAMEBUFFER_TYPE_RGB:
		fb->width = fbtag->width;
		fb->height = fbtag->height;

		fb->pitch = fbtag->pitch;

		fb->framebuffer = fbtag->addr;

		fb->bits_per_pixel = fbtag->bpp;
		fb->bytes_per_pixel = fb->bits_per_pixel >> 3;

		fb->red.position = fbtag->red_field_position;
		fb->red.mask = (1 << fbtag->red_mask_size) - 1;

		fb->green.position = fbtag->green_field_position;
		fb->green.mask = (1 << fbtag->green_mask_size) - 1;

		fb->blue.position = fbtag->blue_field_position;
		fb->blue.mask = (1 << fbtag->blue_mask_size) - 1;

		fb->reserved.position = 24;
		fb->reserved.mask = 0xff;

		k_paging_reserve_pages(fb->framebuffer, fb->height * fb->pitch);

		break;

	case K_MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT:
		fb->width = fbtag->width;
		fb->height = fbtag->height;

		break;

	default:
		return K_ERROR_NOT_FOUND;
	}

	return K_ERROR_NONE;
}

k_error_t k_scan_multiboot_tags(k_uint32_t ebx, int type, k_error_t (*callback)
		(void *, void *), void *data)
{
	struct k_multiboot2_tag *tag;

	tag = (struct k_multiboot2_tag *)(ebx + 8);

	while (tag->type != K_MULTIBOOT2_TAG_TYPE_END) {
		if (tag->type == type)
			return callback((void *)tag, data);

		tag = (struct k_multiboot2_tag *)((k_uint8_t *)tag + ((tag->size + 0x7) & ~0x7));
	}

	return K_ERROR_NOT_FOUND;
}

#endif

void k_main(k_uint32_t eax, k_uint32_t ebx)
{
	k_uint32_t page_table, heap;
#if 0
	struct k_fb_info fb;
	void *rsdp = NULL;
#endif

	if (eax != K_MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	k_idt_init();

	page_table = K_ALIGN_UP(K_MAX((k_uint32_t)__k_end, ebx + *(k_uint32_t *)ebx), 0x1000);
	k_paging_table_set_start(page_table);

	k_paging_reserve_pages(0x0, 1 << 20);
	k_paging_reserve_pages((k_uint32_t)__k_start, __k_end - __k_start);
	k_paging_reserve_pages(ebx, *(k_uint32_t *)ebx);

#if 0
	k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_MODULE, k_get_initramfs, NULL);

#ifdef K_CONFIG_BIOS
	k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_MMAP, k_reserve_reserved_pages, NULL);
#elif K_CONFIG_UEFI
	k_error_t error;

	error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_ACPI_OLD, k_get_old_acpi, &rsdp);
	if (error)
		k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_ACPI_NEW, k_get_new_acpi, &rsdp);

	k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_EFI_MMAP, k_reserve_reserved_pages, NULL);
#endif

	k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_FRAMEBUFFER, k_get_fb_info, &fb);

	// QEMU doesn't report APIC BIOS e820 memory map.
	k_paging_reserve_pages(0xfee00000, 0x1000);

	k_paging_init();

	heap = page_table + 0x1000 + 0x400 * 0x1000;

	k_x86_init(heap, &fb, rsdp);
#endif
}

