int k_log2_arch(unsigned n)
{
	asm volatile(
		"bsr %1, %0"
		: "=r" (n)
		: "rm" (n)
	);

	return n;
}
