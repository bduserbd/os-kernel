#ifndef K_IPV4_H
#define K_IPV4_H

#include "kernel/include/types.h"
#include "kernel/include/network/buffer.h"

#define K_IPV4_VERSION	4

#define K_IPV4_TOS_ROUTINE		0
#define K_IPV4_TOS_PRIORITY		1
#define K_IPV4_TOS_IMMEDIATE		2
#define K_IPV4_TOS_FLASH		3
#define K_IPV4_TOS_FLASH_OVERRIDE	4
#define K_IPV4_TOS_CRITICAL		5
#define K_IPV4_TOS_INTERNETWORK_CONTROL	6
#define K_IPV4_TOS_NETWORK_CONTROL	7

#define K_IPV4_TOS_D	(1 << 3)
#define K_IPV4_TOS_T	(1 << 4)
#define K_IPV4_TOS_R	(1 << 5)

#define K_IPV4_TTL	64

#define K_IPV4_PROTOCOL_ICMP	1
#define K_IPV4_PROTOCOL_TCP	6
#define K_IPV4_PROTOCOL_UDP	17

struct k_ipv4_header {
	__u8	ihl : 4;
	__u8	version : 4;
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

void k_ipv4_build_packet(struct k_network_buffer *, k_uint8_t [6]);

#endif

