#ifndef K_ELF_LOADER_H
#define K_ELF_LOADER_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/elf/elf.h"
#include "kernel/include/modules/module.h"

#ifdef K_BITS_32
#define Elf(x) Elf32_##x
#endif

#define K_ELF_MAX_SECTION_NAME_LENGTH	32

k_error_t k_elf_check(Elf(Ehdr) *, k_size_t);
k_error_t k_elf_load_image(Elf(Ehdr) *, struct k_module *);
void *k_elf_section_by_name(Elf(Ehdr) *, const char *);

#endif

