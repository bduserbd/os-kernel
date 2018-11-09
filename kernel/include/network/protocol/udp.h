#ifndef K_UDP_H
#define K_UDP_H

#include "kernel/include/types.h"

struct k_udp_header {
	__u16	src_port;
	__u16	dest_port;
	__u16	length;
	__u16	checksum;
} __attribute__((packed));

#endif

