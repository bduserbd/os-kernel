#include "include/gdt-descriptor.S"

	.code16
	.globl	_start
_start:
	cli

	/* Got to protected mode. */
	lgdtl	ap_first_gdt_desc

	movl	%cr0, %ebx
	orl	$0x1, %ebx
	movl	%ebx, %cr0

	ljmpl	$0x8, $ap_in_protected_mode

	.code32
ap_in_protected_mode:
	movw	$0x10, %bx
	movw	%bx, %ds
	movw	%bx, %es
	movw	%bx, %fs
	movw	%bx, %gs
	movw	%bx, %ss

	# Specific paramters per CPU
	movl	$0x8180, %ebx

	# Load cr3
	movl	12(%ebx), %eax
	movl	%eax, %cr3

	# Enable paging
	movl	%cr0, %eax
	orl	$0x80010000, %eax
	movl	%eax, %cr0

	movl	4(%ebx), %esp
	pushl	8(%ebx)

	movl	(%ebx), %eax
	call	*%eax

loop:
	hlt
	jmp	loop

	/* Basic GDT. */
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

	/* Just give it the impression of being boot sector .. */
	.=0x200-2
	.byte	0x55
	.byte	0xaa

