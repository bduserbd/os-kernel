#include "include/init.h"
#include "include/idt.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/grub/multiboot2.h"

void puts(const char *);
void puthex(k_uint32_t);

extern __u8 __k_start[];
extern __u8 __k_end[];

void k_paging_init(void);
void k_paging_table_set_start(k_uint32_t);
void k_paging_reserve_pages(k_uint32_t, k_uint32_t);

#ifdef K_CONFIG_BIOS
void k_reserve_reserved_pages(struct k_multiboot2_tag_mmap *mmap)
{
	k_uint32_t i;
	struct k_multiboot2_mmap_entry *entry;

	if (mmap->entry_size != sizeof(*entry))
		return;

	entry = &mmap->entries[0];
	for (i = 0; i < mmap->size / mmap->entry_size; i++, entry++)
		if (entry->type == K_MULTIBOOT2_MEMORY_RESERVED ||
				entry->type == K_MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE)
			if (entry->addr + entry->len > (1 << 20))
				k_paging_reserve_pages(entry->addr & 0xffffffff, entry->len & 0xffffffff);
}
#endif

#ifdef K_CONFIG_UEFI
void k_set_gop_framebuffer(struct k_multiboot2_tag_framebuffer *fb)
{
	if (fb->framebuffer_type != K_MULTIBOOT2_FRAMEBUFFER_TYPE_RGB)
		return;

	while (1) ;
}
#endif

void k_scan_multiboot_tags(k_uint32_t ebx)
{
	struct k_multiboot2_tag *tag;

	tag = (struct k_multiboot2_tag *)(ebx + 8);

	while (tag->type != K_MULTIBOOT2_TAG_TYPE_END) {
		switch (tag->type) {
		case K_MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO:
			break;

#ifdef K_CONFIG_BIOS
		case K_MULTIBOOT2_TAG_TYPE_MMAP:
			k_reserve_reserved_pages((void *)tag);
			break;
#endif

#ifdef K_CONFIG_UEFI
		case K_MULTIBOOT2_TAG_TYPE_FRAMEBUFFER:
			k_set_gop_framebuffer((void *)tag);
			break;

		case K_MULTIBOOT2_TAG_TYPE_EFI_MMAP:
			break;
#endif
		}

		tag = (struct k_multiboot2_tag *)((k_uint8_t *)tag + ((tag->size + 0x7) & ~0x7));
	}
}

void k_main(k_uint32_t eax, k_uint32_t ebx)
{
	k_uint32_t page_table, heap;

	if (eax != K_MULTIBOOT2_BOOTLOADER_MAGIC)
		return;

	k_idt_init();

	page_table = K_ALIGN_UP(K_MAX((k_uint32_t)__k_end, ebx + *(k_uint32_t *)ebx), 0x1000);
	k_paging_table_set_start(page_table);

	k_paging_reserve_pages(0x0, 1 << 20);

	k_paging_reserve_pages((k_uint32_t)__k_start, (k_uint32_t)(__k_end - __k_start));

	k_paging_reserve_pages(ebx, *(k_uint32_t *)ebx);

	k_scan_multiboot_tags(ebx);

	// QEMU doesn't report APIC memory map.
	k_paging_reserve_pages(0xfee00000, 0x1000);

	k_paging_init();

	heap = page_table + 0x1000 + 0x400 * 0x1000;
	k_x86_init(heap);
}

