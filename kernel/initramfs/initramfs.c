#include "include/initramfs/initramfs.h"
#include "include/fs/cpio/cpio.h"
#include "include/modules/loader.h"
#include "include/string.h"
#include "include/video/print.h"

static k_error_t k_cpio_newc_get_field(const char *s, k_uint32_t *field)
{
	int i;

	*field = 0x0;

	for (i = 0; i < 8; i++) {
		*field <<= 4;

		if (s[i] >= '0' && '9' >= s[i])
			*field += s[i] - '0';
		else {
			if (s[i] >= 'A' && 'Z' >= s[i])
				*field += s[i] - 'A';
			else if (s[i] >= 'a' && 'z' >= s[i])
				*field += s[i] - 'a';
			else
				return K_ERROR_INVALID_PARAMETER;

			*field += 0xA;
		}
	}

	return K_ERROR_NONE;
}

static void k_cpio_newc_scan_files(struct k_cpio_newc *cpio)
{
	k_error_t error;
	k_uint32_t namesize, filesize;

	while (1) {
		error = k_cpio_newc_get_field(cpio->namesize, &namesize);
		if (error)
			return;

		error = k_cpio_newc_get_field(cpio->filesize, &filesize);
		if (error)
			return;

		if (!k_strncmp(cpio->filename, K_CPIO_NEWC_TRAILER, 10))
			break;

		if (filesize) {
			k_printf("%s %x %x | ", cpio->filename, namesize, filesize);

			if (!k_strncmp(cpio->filename, K_INITRAMFS_MODULES_DIR,
						sizeof(K_INITRAMFS_MODULES_DIR) - 1))
				k_loader((k_uint8_t *)cpio + K_ALIGN_UP(sizeof(struct k_cpio_newc) +
							namesize, 0x4), filesize);
		}

		cpio = (struct k_cpio_newc *)((char *)cpio +
				K_ALIGN_UP(sizeof(struct k_cpio_newc) +
					namesize + filesize, 0x4));
	}
}

void k_initramfs_get_info(k_uint32_t start, k_uint32_t length)
{
	struct k_initramfs_header *initramfs;

	initramfs = (void *)start;

	if (initramfs->magic != K_INITRAMFS_MAGIC)
		return;

	if (!k_strncmp(initramfs->cpio, K_CPIO_NEWC_MAGIC, 6))
		k_cpio_newc_scan_files((struct k_cpio_newc *)initramfs->cpio);
}

