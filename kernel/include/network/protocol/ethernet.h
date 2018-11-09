#ifndef K_ETHERNET_H
#define K_ETHERNET_H

#include "kernel/include/types.h"

#define K_MAC_LENGTH	6

#define K_ETHERNET_PROTOCOL_IP	0x800
#define K_ETHERNET_PROTOCOL_ARP	0x806

struct k_ethernet_header {
	__u8	mac_dest[K_MAC_LENGTH];
	__u8	mac_src[K_MAC_LENGTH];
	__u16	protocol;
} __attribute__((packed));

#endif

