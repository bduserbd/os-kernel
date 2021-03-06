#ifndef K_CPIO_H
#define K_CPIO_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

#define K_CPIO_NEWC_MAGIC	"070701"

#define K_CPIO_NEWC_TRAILER	"TRAILER!!!"

struct k_cpio_newc {
	char magic[6];
	char ino[8];
	char mode[8];
	char uid[8];
	char gid[8];
	char nlink[8];
	char mtime[8];
	char filesize[8];
	char devmajor[8];
	char devminor[8];
	char rdevmajor[8];
	char rdevminor[8];
	char namesize[8];
	char check[8];
	char filename[0];
} __attribute__((packed));

#endif

