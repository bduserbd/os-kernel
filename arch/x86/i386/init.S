#include "include/i386/paging.h"
#include "include/gdt.h"

	.text
	.code32

	.globl	k_paging_early_reserve
k_paging_early_reserve:
	pushl	$0
	pushl	$0
	pushl	%ecx
	pushl	%eax
	pushl	K_PHYSICAL_ADDRESS(k_page_table)
	call	k_paging_reserve_pages_ptr
	addl	$0x14, %esp

	ret

	.globl	k_memory_mode_init
k_memory_mode_init:
	leal	1f, %eax
	jmp	*%eax
1:
	addl	$K_IMAGE_BASE, %esp

	lgdt	first_gdt_desc
	movl	$K_GDT_KERNEL_DATA_SELECTOR, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	jmp	$K_GDT_KERNEL_CODE_SELECTOR, $our_protected_mode
	.globl	our_protected_mode
our_protected_mode:
	lidt	first_idt

	pushl	$0
	popf

	addl	$K_IMAGE_BASE, k_multiboot_info_ptr

	call	k_paging_remove_identity_map

	/* Main flow. */
	#call	k_main
1:
	jmp	1b

	.data
#include "include/gdt-descriptor.S"
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

	.p2align 3
first_idt:
	.word   0
	.long   0

