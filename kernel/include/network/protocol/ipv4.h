#ifndef K_IPV4_H
#define K_IPV4_H

#include "kernel/include/types.h"

#define K_IPV4_PROTOCOL_ICMP	1
#define K_IPV4_PROTOCOL_TCP	6
#define K_IPV4_PROTOCOL_UDP	17

struct k_ipv4_header {
	__u8	version : 4;
	__u8	ihl : 4;
	__u8	tos;
	__u16	total_length;
	__u16	id;
	__u16	fragment_offset;
	__u8	ttl;
	__u8	protocol;
	__u16	checksum;
	__u32	src_ip;
	__u32	dest_ip;
} __attribute__((packed));

#endif

