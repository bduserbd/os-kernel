#include "include/ap.h"
#include "include/gdt-descriptor.S"
#include "include/cr0.h"
#include "include/cr4.h"
#include "include/msr.h"

	.code16
	.globl	_start
_start:
	cli

	# PAE
	movl	%cr4, %eax
	orl	$K_CR4_PAE, %eax
	movl	%eax, %cr4

	# Specific paramters per CPU
	movl	$K_AP_START_INFO, %ebx

	# Load cr3
	movl	(%ebx), %eax
	movl	%eax, %cr3

	# Long mode & NXE
	movl	$K_MSR_IA32_EFER, %ecx
	rdmsr
	orl	$(K_MSR_IA32_EFER_LME | K_MSR_IA32_EFER_NXE), %eax
	wrmsr

	# Enable paging & set protected mode
	movl	%cr0, %eax
	orl	$(K_CR0_PG | K_CR0_PE), %eax
	movl	%eax, %cr0

	# Load GDT
	lgdt	ap_gdt_desc
	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	jmp	$0x8, $ap_in_long_mode

	.code64
ap_in_long_mode:
	# Specific paramters per CPU
	movq	$K_AP_START_INFO, %rbx

	movq	12(%rbx), %rsp

	movq	4(%rbx), %rax
	call	*%rax

loop:
	hlt
	jmp	loop

	# Basic GDT
	.p2align	3
ap_gdt:
	K_GDT_ENTRY g=0
	K_GDT_ENTRY base=0x0 limit=0x0 type=0xb s=1 dpl=0 p=1 l=1 g=1
	K_GDT_ENTRY base=0x0 limit=0x0 type=0x3 s=1 dpl=0 p=1
ap_gdt_end:

	.p2align	3
ap_gdt_desc:
	.word	ap_gdt_end-ap_gdt-1
	.quad	ap_gdt

