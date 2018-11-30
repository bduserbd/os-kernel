#ifndef K_DHCP_H
#define K_DHCP_H

#include "kernel/include/network/network.h"
#include "kernel/include/network/protocol/udp.h"

#define K_BOOTP_REQUEST	1
#define K_BOOTP_REPLY	2

#define K_HARDWARE_TYPE_ETHERNET	1

#define K_DHCP_FLAGS_BROADCAST	(1 << 15)
#define K_DHCP_FLAGS_UNICAST	(0 << 15)

#define K_OPTION_PAD		0
#define K_OPTION_DHCP_MSG_TYPE	53

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

void k_dhcp_build_packet(struct k_network_buffer *, int);
k_error_t k_dhcp_rx(struct k_network_buffer *);

#endif

