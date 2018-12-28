#include "kernel/include/elf/elf-loader.h"
#include "kernel/include/video/print.h"

k_error_t k_elf_arch_check(Elf(Ehdr) *elf, k_size_t size)
{
	if (elf->e_ident[EI_CLASS] != ELFCLASS64 ||
			elf->e_ident[EI_DATA] != ELFDATA2LSB ||
			elf->e_machine != EM_X86_64)
		return K_ERROR_INVALID_MODULE_HEADER;

	return K_ERROR_NONE;
}

k_error_t k_elf_arch_relocate_section(k_uint8_t *applied_section,
		const Elf(Rel) *rel, const Elf(Sym) *symbol)
{
	return K_ERROR_NOT_IMPLEMENTED;
}

k_error_t k_elf_arch_relocate_addend_section(k_uint8_t *applied_section,
		const Elf(Rela) *rela, const Elf(Sym) *symbol)
{
	Elf(Word) *rela32;
	Elf(Xword) *rela64;

	rela32 = (Elf(Word) *)(applied_section + rela->r_offset);
	rela64 = (Elf(Xword) *)rela32;

	switch (ELF_R_TYPE(rela->r_info)) {
	case R_X86_64_64:
		*rela64 += symbol[ELF_R_SYM(rela->r_info)].st_value + rela->r_addend;
		break;

	case R_X86_64_PC32:
		*rela32 = (k_int32_t)*rela32 +
				symbol[ELF_R_SYM(rela->r_info)].st_value + rela->r_addend -
				(unsigned long)rela32;
		break;

	case R_X86_64_32S:
		*rela32 = (k_int32_t)*rela32 +
				symbol[ELF_R_SYM(rela->r_info)].st_value + rela->r_addend;
		break;

	default:
		k_printf("Unhandled relocation");
		break;
	}

	return K_ERROR_NONE;
}

