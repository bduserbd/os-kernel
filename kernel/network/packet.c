#include "include/network/packet.h"
#include "include/network/protocol/ethernet.h"
#include "include/mm/mm.h"
#include "include/modules/export-symbol.h"

static struct k_network_packet k_network_packet_supported[] = {
	{
		.type = K_NETWORK_PACKET_ETHERNET,
		.callback = k_ethernet_rx,
	},

	{ 0 },
};

struct k_network_packet *k_network_packet_get(int type)
{
	int i;

	for (i = 0; k_network_packet_supported[i].type != K_NETWORK_PACKET_INVALID; i++)
		if (k_network_packet_supported[i].type == type)
			return &k_network_packet_supported[i];

	return NULL;
}
K_EXPORT_FUNC(k_network_packet_get);

