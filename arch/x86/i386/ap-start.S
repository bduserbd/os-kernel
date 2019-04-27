#include "include/ap.h"
#include "include/gdt-descriptor.S"
#include "include/cr0.h"

	.code16
	.globl	_start
_start:
	cli

	# Goto to protected mode
	lgdtl	ap_first_gdt_desc

	movl	%cr0, %eax
	orl	$K_CR0_PE, %eax
	movl	%eax, %cr0

	ljmpl	$0x8, $ap_in_protected_mode

	.code32
ap_in_protected_mode:
	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	# Specific paramters per CPU
	movl	$K_AP_START_INFO, %ebx

	# Load cr3
	movl	(%ebx), %eax
	movl	%eax, %cr3

	# Enable paging
	movl	%cr0, %eax
	orl	$K_CR0_PG, %eax
	movl	%eax, %cr0

	movl	8(%ebx), %esp

	movl	4(%ebx), %eax
	call	*%eax

loop:
	hlt
	jmp	loop

	# Basic GDT
	.p2align	3
ap_first_gdt:
	K_GDT_ENTRY
	K_GDT_ENTRY base=0 limit=0xfffff type=0xb s=1 p=1 db=1 g=1
	K_GDT_ENTRY base=0 limit=0xfffff type=0x3 s=1 p=1 db=1 g=1
ap_first_gdt_end:

	.p2align	3
ap_first_gdt_desc:
	.word	ap_first_gdt_end-ap_first_gdt-1
	.long	ap_first_gdt

