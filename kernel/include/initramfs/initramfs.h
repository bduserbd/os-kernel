#ifndef K_INITRAMFS_H
#define K_INITRAMFS_H

#include "kernel/include/types.h"

#define K_INITRAMFS_MAGIC	0x53444f4d

struct k_initramfs_header {
	k_uint32_t magic;
	k_uint32_t length;
	k_uint32_t crc32;
	k_uint8_t cpio[0];
} __attribute__((packed));

void k_initramfs_get_info(k_uint32_t, k_uint32_t);

#endif

