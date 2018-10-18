#include "include/modules/loader.h"
#include "include/elf/elf-loader.h"
#include "include/video/print.h"

void k_loader(const k_uint8_t *buf, k_size_t size)
{
	k_elf_check((Elf(Ehdr) *)buf, size);
}

