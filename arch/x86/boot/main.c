#include "include/init.h"
#include "kernel/include/grub/multiboot2.h"

void puts(const char *);
void puthex(k_uint32_t);

extern __u8 __k_start[];
extern __u8 __k_end[];

void k_paging_init(void);
void k_paging_table_set_start(k_uint32_t);
void k_paging_reserve_pages(k_uint32_t, k_uint32_t);

void k_reserve_reserved_pages(struct k_multiboot2_tag *tag)
{
	k_uint32_t i;
	struct k_multiboot2_tag_mmap *mmap;
	struct k_multiboot2_mmap_entry *entry;

	mmap = (void *)tag;
	if (mmap->entry_size != sizeof(*entry))
		return;

	entry = &mmap->entries[0];
	for (i = 0; i < mmap->size / mmap->entry_size; i++, entry++)
		if (entry->type == K_MULTIBOOT2_MEMORY_RESERVED ||
				entry->type == K_MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE)
			if (entry->addr + entry->len > (1 << 20))
				k_paging_reserve_pages(entry->addr & 0xffffffff, entry->len & 0xffffffff);
}

void k_scan_multiboot_tags(k_uint32_t ebx)
{
	struct k_multiboot2_tag *tag;

	tag = (struct k_multiboot2_tag *)(ebx + 8);

	while (tag->type != K_MULTIBOOT2_TAG_TYPE_END) {
		if (tag->type == K_MULTIBOOT2_TAG_TYPE_MMAP)
			k_reserve_reserved_pages(tag);

		tag = (struct k_multiboot2_tag *)((k_uint8_t *)tag + ((tag->size + 0x7) & ~0x7));
	}
}

void k_main(k_uint32_t eax, k_uint32_t ebx)
{
	k_uint32_t page_table;

	if (eax != K_MULTIBOOT2_BOOTLOADER_MAGIC)
		return;

	page_table = K_ALIGN_UP(K_MAX((k_uint32_t)__k_end, ebx + *(k_uint32_t *)ebx), 0x1000);
	k_paging_table_set_start(page_table);

	k_paging_reserve_pages(0x0, 1 << 20);

	k_paging_reserve_pages((k_uint32_t)__k_start, (k_uint32_t)(__k_end - __k_start));

	k_paging_reserve_pages(ebx, *(k_uint32_t *)ebx);

	k_scan_multiboot_tags(ebx);

	k_paging_init();

	k_x86_init();
}

