#include "include/network/protocol/dhcp.h"
#include "include/network/protocol/ethernet.h"
#include "include/string.h"

static k_uint8_t k_dhcp_magic_cookie[4] = { 99, 130, 83, 99 };

static void k_dhcp_build_options(struct k_dhcp_header *dhcp, int options_length)
{
	int i = 4;
#define I	(i++)

	k_memcpy(dhcp->options, k_dhcp_magic_cookie, sizeof(k_dhcp_magic_cookie));

	dhcp->options[I] = 53;
	dhcp->options[I] = 1;
	dhcp->options[I] = 1;

	dhcp->options[I] = 12; dhcp->options[I] = 1; dhcp->options[I] = 'b';

	dhcp->options[I] = 55;
	dhcp->options[I] = 18;
	dhcp->options[I] = 1;
	dhcp->options[I] = 28;
	dhcp->options[I] = 2;
	dhcp->options[I] = 3;
	dhcp->options[I] = 15;
	dhcp->options[I] = 6;
	dhcp->options[I] = 119;
	dhcp->options[I] = 12;
	dhcp->options[I] = 44;
	dhcp->options[I] = 47;
	dhcp->options[I] = 26;
	dhcp->options[I] = 121;
	dhcp->options[I] = 42;
	dhcp->options[I] = 121;
	dhcp->options[I] = 249;
	dhcp->options[I] = 33;
	dhcp->options[I] = 252;
	dhcp->options[I] = 42;

	dhcp->options[I] = 0xff;
}

void k_dhcp_build_packet(struct k_network_buffer *buffer, int options_length)
{
	struct k_dhcp_header *dhcp;

	k_network_buffer_adjust(buffer, sizeof(struct k_dhcp_header) + options_length);

	dhcp = (void *)buffer->packet_start;

	dhcp->op = K_BOOTP_REQUEST;
	dhcp->htype = K_HARDWARE_TYPE_ETHERNET;
	dhcp->hlen = K_MAC_LENGTH; // TODO: Generic HW address length.
	dhcp->hops = 0;
	dhcp->xid = k_cpu_to_be32(0);
	dhcp->secs = k_cpu_to_be16(0);
	dhcp->flags = k_cpu_to_be16(K_DHCP_FLAGS_UNICAST);

	dhcp->client_ip = K_IPV4(0, 0, 0, 0);
	dhcp->your_ip = K_IPV4(0, 0, 0, 0);
	dhcp->server_ip = K_IPV4(0, 0, 0, 0);
	dhcp->relay_ip = K_IPV4(0, 0, 0, 0);

	k_memcpy(dhcp->client_hw_address, buffer->card->hw_address, dhcp->hlen);

	k_memset(dhcp->server_name, 0, sizeof(dhcp->server_name));
	k_memset(dhcp->boot_file, 0, sizeof(dhcp->boot_file));

	k_dhcp_build_options(dhcp, options_length);

	k_udp_build_packet(buffer, sizeof(struct k_dhcp_header) + options_length,
			K_PORT_DHCP_CLIENT, K_PORT_DHCP_SERVER, K_IPV4_BROADCAST);
}

k_error_t k_dhcp_rx(struct k_network_buffer *buffer)
{
	k_printf("DHCP");

	return K_ERROR_NONE;
}

