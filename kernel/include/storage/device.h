#ifndef K_STORAGE_DEVICE_H
#define K_STORAGE_DEVICE_H

#include "kernel/include/types.h"

typedef k_uint64_t	k_sector_t;

#define K_SECTOR_SIZE	512

struct k_partition {
	k_sector_t start;
	k_sector_t sectors;
};

struct k_disk {
	int major;
	int minor;

	int number_of_partitions;
};

#endif

