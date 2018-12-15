int k_log2_arch(unsigned n)
{
	asm volatile(
		"bsr %1, %0"
		: "=r" (n)
		: "rm" (n)
	);

	return n;
}

int k_bit_scan_forward_arch(unsigned n)
{
	asm volatile(
		"bsf %1, %0"
		: "=r" (n)
		: "rm" (n)
	);

	return n;
}

