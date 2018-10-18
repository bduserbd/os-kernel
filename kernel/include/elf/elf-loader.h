#ifndef K_ELF_LOADER_H
#define K_ELF_LOADER_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/elf/elf.h"

#ifdef K_BITS_32
#define Elf(x) Elf32_##x
#endif

k_error_t k_elf_check(Elf(Ehdr) *, k_size_t);

#endif

