#ifndef K_INITRAMFS_H
#define K_INITRAMFS_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

#define K_INITRAMFS_MAGIC	0x53444f4d

#define K_INITRAMFS_OBJECTS_DIR	"objects/"
#define K_INITRAMFS_MODULES_DIR	(K_INITRAMFS_OBJECTS_DIR "modules/")

struct k_initramfs_header {
	k_uint32_t magic;
	k_uint32_t length;
	k_uint32_t crc32;
	char cpio[0];
} __attribute__((packed));

void k_initramfs_get_info(k_uint32_t, k_uint32_t);

#endif

