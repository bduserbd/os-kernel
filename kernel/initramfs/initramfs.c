#include "include/initramfs/initramfs.h"
#include "include/fs/cpio/cpio.h"
#include "include/video/print.h"

void k_initramfs_get_info(k_uint32_t start, k_uint32_t length)
{
	struct k_initramfs_header *initramfs;

	initramfs = (void *)start;

	if (initramfs->magic != K_INITRAMFS_MAGIC)
		return;

	k_printf("Initramfs here %s", initramfs->cpio);
}

