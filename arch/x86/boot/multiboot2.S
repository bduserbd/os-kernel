#include "include/gdt-descriptor.S"
#include "include/grub/multiboot2.h"

#define STACK_SIZE	0x1000

# The Multiboot2 header.
	.section ".multiboot2", "a"

header_start:
	.long	MULTIBOOT2_HEADER_MAGIC
	.long	MULTIBOOT2_ARCHITECTURE_I386
	.long	header_end - header_start
	.long	-(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + (header_end - header_start))

end_tag:
	.word	MULTIBOOT2_TAG_TYPE_END
	.word	0
	.long	8

header_end:

	.text
	.globl	_start
_start:
	movl	$(stack + STACK_SIZE), %esp

	pushl	$0
	popf

	pushl	%ebx
	pushl	%eax

	/* Set segments & GDT. */
	lgdt	first_gdt_desc
	movl	$0x10, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	call	main

die:
	hlt
	jmp die

	.data

/* Basic GDT. */
	.p2align 3
first_gdt:
	BL_GDT_ENTRY
	BL_GDT_ENTRY base=0 limit=0xfffff type=0xb s=1 p=1 db=1 g=1
	BL_GDT_ENTRY base=0 limit=0xfffff type=0x3 s=1 p=1 db=1 g=1
first_gdt_end:

	.p2align 3
first_gdt_desc:
	.word	first_gdt_end-first_gdt-1
	.long	first_gdt

	.comm	stack, STACK_SIZE
