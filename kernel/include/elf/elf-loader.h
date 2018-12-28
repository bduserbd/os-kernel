#ifndef K_ELF_LOADER_H
#define K_ELF_LOADER_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/elf/elf.h"

#ifdef K_BITS_32
#define Elf(x) Elf32_##x
#elif K_BITS_64
#define Elf(x) Elf64_##x
#endif

#define K_ELF_MAX_SECTION_NAME_LENGTH	32

#endif

