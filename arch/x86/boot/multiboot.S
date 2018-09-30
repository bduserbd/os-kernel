#include "kernel/include/grub/multiboot.h"

# The Multiboot header
	.section ".multiboot", "a"

	.p2align 2
header_start:
	.long	K_MULTIBOOT_HEADER_MAGIC
	.long	0
	.long	-(K_MULTIBOOT_HEADER_MAGIC + 0)

header_end:

	.text
	.globl	_start
_start:
	jmp	_real_start

