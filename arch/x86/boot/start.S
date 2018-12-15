#include "include/gdt.h"
#include "include/paging.h"

	.text
	.code32
	.globl	_real_start
_real_start:
	cli

	movl    $K_PHYSICAL_ADDRESS(__k_stack_start), %esp

	/* Grub info. */
	movl	%eax, K_PHYSICAL_ADDRESS(k_multiboot_magic_ptr)
	movl	%ebx, K_PHYSICAL_ADDRESS(k_multiboot_info_ptr)

	/* Basic CPU reqiurements. */
	call    k_cpu_is_valid
	testl   %eax, %eax
	jnz     die

	/* Boot setup. */
	call	k_multiboot_is_valid
	testl	%eax, %eax
	jnz	die

	call	k_alloc_boot_page_table
	testl	%eax, %eax
	jz	die

	pushl	%eax
	call	k_paging_table_set_start
	addl	$0x4, %esp

	movl	$K_PHYSICAL_ADDRESS(__k_start), %eax
	movl	$K_PHYSICAL_ADDRESS(__k_end), %ecx
	subl	%eax, %ecx
	call	k_paging_early_reserve

	call	k_paging_identity_map

	call	k_paging_init

	call	k_memory_mode_init

die:
	hlt
	jmp die

	.globl k_multiboot_magic_ptr
k_multiboot_magic_ptr:
	.long	0

	.globl k_multiboot_info_ptr
k_multiboot_info_ptr:
	.long	0

	.globl k_initramfs_start
k_initramfs_start:
	.long	0

	.globl k_initramfs_length
k_initramfs_length:
	.long	0

	.p2align 5
	.section ".user", "a"
	.skip	K_CONFIG_USER_MODE_EXECUTABLE

/* The kernel stack. */
	.p2align 5
	.section ".kernel_stack", "a"
	.skip   K_CONFIG_STACK_SIZE

#ifdef K_CONFIG_SMP
/* Empty space for AP start. */
	.section ".ap_start", "a"
	.skip   0x200
#endif

