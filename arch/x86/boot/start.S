#include "include/gdt-descriptor.S"
#include "include/gdt.h"
#include "include/paging.h"

	.text
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

#if 0
	pushl	$0
	pushl	$0
	movl	$__k_end, %eax
	subl	$__k_start, %eax
	pushl	%eax
	pushl	$K_PHYSICAL_ADDRESS(__k_start)
	pushl	K_PHYSICAL_ADDRESS(k_page_table);
	call	k_paging_reserve_pages_ptr
	addl	$0x10, %esp

	call	k_paging_identity_map

	call	k_paging_init

	lea	1f, %eax
	jmp	*%eax
1:

	addl	$K_IMAGE_BASE, %esp

	/* Set segments & GDT & IDT. */
	lgdt	first_gdt_desc
	movl    $K_GDT_KERNEL_DATA_SELECTOR, %eax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	movw    %ax, %ss

	jmp	$K_GDT_KERNEL_CODE_SELECTOR, $our_protected_mode
our_protected_mode:

	lidt first_idt

	pushl	$0
	popf

	addl	$K_IMAGE_BASE, k_multiboot_info_ptr

	call	k_paging_remove_identity_map
#endif

	/* Main flow. */
	#call	k_main

die:
	hlt
	jmp die


#if 0
	.data
/* Basic GDT. */
	.p2align 3
first_gdt:
	K_GDT_ENTRY
	K_GDT_ENTRY base=0x0 limit=0xfffff type=0xa s=1 dpl=0 p=1 db=1 g=1
	K_GDT_ENTRY base=0x0 limit=0xfffff type=0x2 s=1 dpl=0 p=1 db=1 g=1
first_gdt_end:

	.p2align 3
first_gdt_desc:
	.word   first_gdt_end-first_gdt-1
	.long   first_gdt

/* Just set IDT. */
	.p2align 3
first_idt:
	.word   0
	.long   0
#endif

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

