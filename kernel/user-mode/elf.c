#include "include/elf/elf-loader.h"
#include "include/video/print.h"
#include "include/mm/buddy.h"
#include "include/string.h"

k_error_t k_elf_arch_check(Elf(Ehdr) *elf, k_size_t size);

k_error_t k_elf_user_check(Elf(Ehdr) *elf, k_size_t *size)
{
        k_error_t error;

	*size = 0;

        if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
                        elf->e_ident[EI_MAG1] != ELFMAG1 ||
                        elf->e_ident[EI_MAG2] != ELFMAG2 ||
                        elf->e_ident[EI_MAG3] != ELFMAG3 ||
                        elf->e_ident[EI_VERSION] != EV_CURRENT ||
                        elf->e_type != ET_EXEC ||
                        elf->e_version != EV_CURRENT)
                return K_ERROR_INVALID_EXECUTABLE_HEADER;

        *size = elf->e_shoff + elf->e_shnum * elf->e_shentsize;

        error = k_elf_arch_check(elf, *size);
        if (error)
                return error;

        return K_ERROR_NONE;
}

void k_paging_map_user(unsigned long, unsigned long, unsigned long);

k_error_t k_elf_user_load(Elf(Ehdr) *elf, void **stack)
{
	int i;
	k_size_t size;
	void *physical;
	const Elf(Phdr) *segment;
	Elf(Addr) min_addr = 0;

	segment = (const Elf(Phdr) *)((k_uint8_t *)elf + elf->e_phoff);

	for (i = 0; i < elf->e_phnum; i++) {
		if (segment[i].p_type != PT_LOAD)
			continue;

		size = K_ALIGN_UP(segment[i].p_memsz, segment[i].p_align);

		physical = k_buddy_user_alloc(size);
		if (!physical)
			return K_ERROR_USER_MEMORY_ALLOCATION_FAILED;

		if (min_addr == 0 || min_addr > segment[i].p_vaddr)
			min_addr = segment[i].p_vaddr;

		k_paging_map_user(segment[i].p_vaddr, (unsigned long)physical, size);

		k_memcpy((void *)segment[i].p_vaddr, (k_uint8_t *)elf + segment[i].p_offset,
				segment[i].p_filesz);
	}

	physical = k_buddy_user_alloc(0x1000);
	if (!physical)
		return K_ERROR_USER_MEMORY_ALLOCATION_FAILED;

	k_paging_map_user(min_addr - 0x1000, (unsigned long)physical, 0x1000);

	*stack = (void *)(min_addr - 1);

	return K_ERROR_NONE;
}

