#include "include/gdt-descriptor.S"
#include "kernel/include/grub/multiboot2.h"

# The Multiboot2 header.
	.section ".multiboot2", "a"

	.p2align 3
header_start:
	.long	K_MULTIBOOT2_HEADER_MAGIC
	.long	K_MULTIBOOT2_ARCHITECTURE_I386
	.long	header_end - header_start
	.long	-(K_MULTIBOOT2_HEADER_MAGIC + K_MULTIBOOT2_ARCHITECTURE_I386 +	\
		(header_end - header_start))

#ifdef K_CONFIG_UEFI
	.p2align 3
framebuffer_tag:
	.word	K_MULTIBOOT2_HEADER_TAG_FRAMEBUFFER
	.word	0
	.long	20
	.long	1024
	.long	768
	.long	32
#endif

	.p2align 3
end_tag:
	.word	K_MULTIBOOT2_HEADER_TAG_END
	.word	0
	.long	8

header_end:

	.text
	.globl	_start
_start:
	jmp	_real_start

