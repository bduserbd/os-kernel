#include "include/gdt-descriptor.S"

	.text
	.globl	_real_start
_real_start:
	cli

	movl    $__k_stack_start, %esp

	/* Grub info. */
	pushl   %ebx
	pushl   %eax

	/* Set segments & GDT & IDT. */
	lgdt	first_gdt_desc
	movl    $0x10, %eax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	movw    %ax, %ss

	jmp	$0x8, $our_protected_mode
our_protected_mode:

	lidt    first_idt

	pushl   $0
	popf

	/* Basic CPU reqiurements. */
	call    k_cpu_valid

	testl   %eax, %eax
	jnz     die

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
	K_GDT_ENTRY base=0x0 limit=0xfffff type=0xa s=1 p=1 db=1 g=1
	K_GDT_ENTRY base=0x0 limit=0xfffff type=0x2 s=1 p=1 db=1 g=1
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

#ifdef K_CONFIG_SMP
/* Empty space for AP start. */
	.section ".ap_start", "a"
	.skip   0x200
#endif

/* The kernel stack. */
	.p2align 5
	.section ".kernel_stack", "a"
	.globl  __k_stack_end
__k_stack_end:
	.skip   K_CONFIG_STACK_SIZE
	.globl  __k_stack_start
__k_stack_start:

