#include "include/gdt-descriptor.S"
#include "kernel/include/grub/multiboot2.h"

#define STACK_SIZE	0x1000

# The Multiboot2 header.
	.section ".multiboot2", "a"

header_start:
	.long	K_MULTIBOOT2_HEADER_MAGIC
	.long	K_MULTIBOOT2_ARCHITECTURE_I386
	.long	header_end - header_start
	.long	-(K_MULTIBOOT2_HEADER_MAGIC + K_MULTIBOOT2_ARCHITECTURE_I386 + (header_end - header_start))

end_tag:
	.word	K_MULTIBOOT2_HEADER_TAG_END
	.word	0
	.long	8

header_end:

	.text
	.globl	_start
_start:
	cli

	movl	$(stack + STACK_SIZE), %esp

	/* Grub info. */
	pushl	%ebx
	pushl	%eax

	/* Set segments & GDT & IDT. */
	lgdt	first_gdt_desc
	movl	$0x10, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	lidt	first_idt

	pushl	$0
	popf

	/* Basic CPU reqiurements. */
	call	k_cpu_valid

	testl	%eax, %eax
	jnz	die

	/* Main flow. */
	call	k_main

die:
	hlt
	jmp die

	.data

/* Basic GDT. */
	.p2align 3
first_gdt:
	K_GDT_ENTRY
	K_GDT_ENTRY base=0 limit=0xfffff type=0xb s=1 p=1 db=1 g=1
	K_GDT_ENTRY base=0 limit=0xfffff type=0x3 s=1 p=1 db=1 g=1
first_gdt_end:

	.p2align 3
first_gdt_desc:
	.word	first_gdt_end-first_gdt-1
	.long	first_gdt

	.p2align 3
first_idt:
	.word	0
	.long	0

	.comm	stack, STACK_SIZE

