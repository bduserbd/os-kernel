#include "include/init.h"
#include "include/paging.h"
#include "include/idt.h"
#include "include/8259a.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/mm/zone.h"
#include "kernel/include/grub/multiboot2.h"
#include "kernel/include/fb/shell.h"
#include "kernel/include/string.h"

extern __u8 __k_start[];
extern __u8 __k_end[];

extern unsigned long *k_multiboot_magic_ptr;
extern unsigned long *k_multiboot_info_ptr;

unsigned long k_multiboot_info_size = 0;

extern unsigned long k_initramfs_start;
extern unsigned long k_initramfs_length;

#if 0
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

/* Functions from here are called when vitual memory is initialized. */
static k_error_t k_get_smbios(void *tag, void *data)
{
	struct k_multiboot2_tag_smbios *smbiostag;

	smbiostag = tag;

	*(void **)data = &smbiostag->tables[0];

	return K_ERROR_NONE;
}

static k_error_t k_get_old_acpi(void *tag, void *data)
{
	struct k_multiboot2_tag_old_acpi *oldacpi;
	struct k_acpi_rsdp *rsdp;

	oldacpi = tag;

	rsdp = (void *)&oldacpi->rsdp[0];
	if (!rsdp->length)
		return K_ERROR_NOT_FOUND;

	*(void **)data = rsdp;

	return K_ERROR_NONE;
}

static k_error_t k_get_new_acpi(void *tag, void *data)
{
	struct k_multiboot2_tag_old_acpi *newacpi;

	newacpi = tag;

	*(void **)data = &newacpi->rsdp[0];

	return K_ERROR_NONE;
}

static k_error_t k_get_basic_memory_info(void *tag, void *data)
{
	struct k_multiboot2_tag_basic_meminfo *meminfo;

	meminfo = tag;

	*(k_uint32_t *)data = meminfo->mem_upper;

	return K_ERROR_NONE;
}

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
#endif

static k_error_t k_reserve_reserved_pages_efi(void *tag, void *data)
{
	int i;
	struct k_multiboot2_tag_efi_mmap *mmaptag;
	struct k_efi_memory_descriptor *entry;

	mmaptag = tag;

	if (mmaptag->descr_size != sizeof(*entry))
		return K_ERROR_NOT_FOUND;

	entry = &mmaptag->efi_mmap[0];
	for (i = 0; i < (mmaptag->size - sizeof(struct k_multiboot2_tag_efi_mmap)) /
			mmaptag->descr_size; i++, entry++)
		if (entry->type == K_EFI_RESERVED_MEMORY_TYPE ||
				entry->type == K_EFI_ACPI_RECLAIM_MEMORY ||
				entry->type == K_EFI_ACPI_MEMORY_NVS)
			k_memory_zone_dma_add(entry->physical_start >> 12, entry->number_of_pages);

	return K_ERROR_NONE;
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

		break;

	default:
		return K_ERROR_NOT_FOUND;
	}

	return K_ERROR_NONE;
}

void k_print_set_output_callback(void (*)(const char *));

k_error_t k_reserve_reserved_pages(void)
{
	k_error_t error;
	k_uint32_t ebx;
	struct k_fb_info fb;

	ebx = (k_uint32_t)k_multiboot_info_ptr;

#if 0
	error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_MMAP, k_reserve_reserved_pages, NULL);
	if (error)
		return;
#endif

	error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_EFI_MMAP,
			k_reserve_reserved_pages_efi, NULL);
	if (error)
		return error;

	// QEMU doesn't report APIC BIOS e820 memory map.
	k_memory_zone_dma_add(0xfee00000 >> 12, 1);

	/* Set also graphics. */
	error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_FRAMEBUFFER,
			k_get_fb_info, &fb);
	if (error)
		return error;

	k_memory_zone_dma_add(fb.framebuffer >> 12, (fb.height * fb.pitch) >> 12);

	k_fb_set_info(&fb);
	k_shell_init();
	k_print_set_output_callback(k_shell_puts);

	return K_ERROR_NONE;
}
#endif

k_error_t k_main(void)
{
#if 0
	k_uint32_t ebx;
	k_uint32_t mem_upper;
	k_error_t error;
	void *smbios = (void *)-1;
	void *rsdp = NULL;

	//k_paging_reserve_pages(K_VIRTUAL_ADDRESS(0x8000), 0x1000);

	ebx = (k_uint32_t)k_multiboot_info_ptr;
	k_paging_reserve_pages(ebx, k_multiboot_info_size);

	k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_SMBIOS, k_get_smbios, &smbios);

	error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_ACPI_OLD,
			k_get_old_acpi, &rsdp);
	if (error) {
		error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_ACPI_NEW,
				k_get_new_acpi, &rsdp);
		if (error)
			return error;
	}

	error = k_scan_multiboot_tags(ebx, K_MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO,
			k_get_basic_memory_info, &mem_upper);
	if (error)
		return error;

	k_paging_build_frame_array(K_KB(mem_upper) >> 12);
	k_buddy_init(K_ALIGN_UP(K_OFFSET_FROM(k_normal_frames, K_FRAME_ARRAY_SIZE), 0x1000));

	/* Page faults and so aren't printed until graphics are set. */
	//k_pic_init();
	k_idt_init();

	k_paging_reserve_pages(k_initramfs_start, k_initramfs_length);

	k_x86_init(smbios, rsdp, k_initramfs_start, k_initramfs_length);
#endif

	return K_ERROR_FAILURE;
}

