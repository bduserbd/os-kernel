#include "include/log2.h"

int k_log2_arch(unsigned);

int k_log2(unsigned n)
{
	if (!n)
		return -1;
	else
		return k_log2_arch(n);
}

