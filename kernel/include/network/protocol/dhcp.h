#ifndef K_DHCP_H
#define K_DHCP_H

#include "kernel/include/types.h"

#define K_BOOTP_REQUEST	1
#define K_BOOTP_REQUEST	2

static k_uint8_t k_dhcp_magic_cookie[4] = { 99, 130, 83, 99 };

#define K_DHCP_DISCOVER	1
#define K_DHCP_OFFER	2
#define K_DHCP_REQUEST	3
#define K_DHCP_DECLINE	4
#define K_DHCP_ACK	5
#define K_DHCP_NAK	6
#define K_DHCP_RELEASE	7
#define K_DHCP_INFORM	8

struct k_dhcp_header {
	__u8	op;
	__u8	htype;
	__u8	hlen;
	__u8	hops;
	__u32	xid;
	__u16	secs;
	__u16	flags;
	__u32	client_ip;
	__u32	your_ip;
	__u32	server_ip;
	__u32	relay_ip;
	__u8	client_hw_address[16];
	__u8	server_name[64];
	__u8	boot_file[128];
	__u8	options[0];
} __attribute__((packed));

#endif

