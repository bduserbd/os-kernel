#include "kernel/include/grub/multiboot.h"
#include "include/paging.h"

# The Multiboot header
	.section ".multiboot", "a"

	.p2align 2
header_start:
	.long	K_MULTIBOOT_HEADER_MAGIC
	.long	0
	.long	-(K_MULTIBOOT_HEADER_MAGIC + 0)
header_end:

	.text
	.code32
	.globl	_start
_start:
	jmp	_real_start

	.globl	k_multiboot_is_valid
k_multiboot_is_valid:
	xorl	%eax, %eax
	movl	$K_MULTIBOOT_BOOTLOADER_MAGIC, %ebx
	cmpl	K_PHYSICAL_ADDRESS(k_multiboot_magic_ptr), %ebx
	setne	%al

	ret

	.globl	k_alloc_boot_page_table
k_alloc_boot_page_table:
	movl	K_PHYSICAL_ADDRESS(k_multiboot_info_ptr), %eax
	movl	%eax, %ebx
	addl	$128, %ebx
	movl	$K_PHYSICAL_ADDRESS(__k_end), %ecx
	cmpl	%ecx, %ebx
	cmovl	%ecx, %ebx

	testb	$K_MULTIBOOT_INFO_MODS, (%eax)
	jz	_exit

	movl	20(%eax), %ecx
	movl	24(%eax), %edx

	testl	%ecx, %ecx
	jz	_exit

	movl	%edx, %esi
	xorl	%edx, %edx

1:
	movl	8(%esi), %edi

	cmpb	$'i', (%edi)
	jne	_exit
	cmpb	$'n', 1(%edi)
	jne	_exit
	cmpb	$'i', 2(%edi)
	jne	_exit
	cmpb	$'t', 3(%edi)
	jne	_exit
	cmpb	$'r', 4(%edi)
	jne	_exit
	cmpb	$'a', 5(%edi)
	jne	_exit
	cmpb	$'m', 6(%edi)
	jne	_exit
	cmpb	$'f', 7(%edi)
	jne	_exit
	cmpb	$'s', 8(%edi)
	jne	_exit
	cmpb	$'.', 9(%edi)
	jne	_exit
	cmpb	$'i', 10(%edi)
	jne	_exit
	cmpb	$'m', 11(%edi)
	jne	_exit
	cmpb	$'g', 12(%edi)
	jne	_exit
	cmpb	$0x0, 13(%edi)
	jne	_exit

	movl	(%esi), %eax
	movl	%eax, K_PHYSICAL_ADDRESS(k_initramfs_start)
	movl	4(%esi), %eax
	subl	K_PHYSICAL_ADDRESS(k_initramfs_start), %eax
	movl	%eax, K_PHYSICAL_ADDRESS(k_initramfs_length)

	movl	4(%esi), %eax
	cmpl	%ebx, %eax
	cmovl	%ebx, %eax

	addl	$0xfff, %eax
	andl	$0xfffff000, %eax
	ret

	incl	%edx
	addl	$16, %esi
	cmpl	%edx, %ecx
	jnz	1b

_exit:
	xorl	%eax, %eax
	ret

