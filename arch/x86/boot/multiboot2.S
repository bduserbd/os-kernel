#include "kernel/include/grub/multiboot2.h"
#include "include/paging.h"

# The Multiboot2 header.
	.section ".multiboot2", "a"

	.p2align 3
header_start:
	.long	K_MULTIBOOT2_HEADER_MAGIC
	.long	K_MULTIBOOT2_ARCHITECTURE_I386
	.long	header_end - header_start
	.long	-(K_MULTIBOOT2_HEADER_MAGIC + K_MULTIBOOT2_ARCHITECTURE_I386 +	\
		(header_end - header_start))

	.p2align 3
framebuffer_tag:
	.word	K_MULTIBOOT2_HEADER_TAG_FRAMEBUFFER
	.word	0
	.long	20
	.long	1024
	.long	768
	.long	32

	.p2align 3
end_tag:
	.word	K_MULTIBOOT2_HEADER_TAG_END
	.word	0
	.long	8

header_end:

	.text
	.code32
	.globl	_start
_start:
	jmp	_real_start

	.globl	k_multiboot_is_valid
k_multiboot_is_valid:
	xorl	%eax, %eax
	movl	$K_MULTIBOOT2_BOOTLOADER_MAGIC, %ebx
	cmpl	K_PHYSICAL_ADDRESS(k_multiboot_magic_ptr), %ebx
	setne	%al

	ret

	.globl	k_alloc_boot_page_table
k_alloc_boot_page_table:
	movl	K_PHYSICAL_ADDRESS(k_multiboot_info_ptr), %eax
	movl	(%eax), %ebx
	movl	%ebx, K_PHYSICAL_ADDRESS(k_multiboot_info_size)
	addl	%eax, %ebx
	movl	$K_PHYSICAL_ADDRESS(__k_end), %ecx
	cmpl	%ecx, %ebx
	cmovl	%ecx, %ebx

	leal	8(%eax), %eax

1:
	movl	(%eax), %ecx

	testl	%ecx, %ecx
	jz	_exit

	cmpl	$K_MULTIBOOT2_TAG_TYPE_MODULE, %ecx
	jne	2f

	cmpb	$'i', 16(%eax)
	jne	2f
	cmpb	$'n', (1+16)(%eax)
	jne	2f
	cmpb	$'i', (2+16)(%eax)
	jne	2f
	cmpb	$'t', (3+16)(%eax)
	jne	2f
	cmpb	$'r', (4+16)(%eax)
	jne	2f
	cmpb	$'a', (5+16)(%eax)
	jne	2f
	cmpb	$'m', (6+16)(%eax)
	jne	2f
	cmpb	$'f', (7+16)(%eax)
	jne	2f
	cmpb	$'s', (8+16)(%eax)
	jne	2f
	cmpb	$'.', (9+16)(%eax)
	jne	2f
	cmpb	$'i', (10+16)(%eax)
	jne	2f
	cmpb	$'m', (11+16)(%eax)
	jne	2f
	cmpb	$'g', (12+16)(%eax)
	jne	2f
	cmpb	$0x0, (13+16)(%eax)
	jne	2f

	movl	8(%eax), %edx
	movl	%edx, K_PHYSICAL_ADDRESS(k_initramfs_start)
	movl	12(%eax), %edx
	subl	K_PHYSICAL_ADDRESS(k_initramfs_start), %edx
	movl	%edx, K_PHYSICAL_ADDRESS(k_initramfs_length)

	movl	12(%eax), %edx
	cmpl	%edx, %ebx
	cmovl	%edx, %ebx

	movl	%ebx, %eax
	addl	$0xfff, %eax
	andl	$~0xfff, %eax
	ret

2:
	movl	4(%eax), %ecx
	addl	$0x7, %ecx
	andl	$~0x7, %ecx
	addl	%ecx, %eax
	jmp	1b

_exit:
	xorl	%eax, %eax
	ret

