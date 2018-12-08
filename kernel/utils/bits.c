#include "include/bits.h"

int k_log2_arch(unsigned);

int k_log2(unsigned n)
{
	if (!n)
		return -1;
	else
		return k_log2_arch(n);
}

int k_bit_scan_forward_arch(unsigned);

int k_bit_scan_forward(unsigned n)
{
	if (!n)
		return -1;
	else
		return k_bit_scan_forward_arch(n);
}

