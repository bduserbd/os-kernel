#include "include/idt.h"

	.text
	.code64

/* Exception that push error code. */
	.macro K_IDT_ERROR_CODE number

	.globl	k_idt_int\number
k_idt_int\number:
	pushq	$\number	
	jmp	1f

	.endm

K_IDT_ERROR_CODE 8
K_IDT_ERROR_CODE 10
K_IDT_ERROR_CODE 11
K_IDT_ERROR_CODE 12
K_IDT_ERROR_CODE 13
K_IDT_ERROR_CODE 14
K_IDT_ERROR_CODE 17
K_IDT_ERROR_CODE 30

/* Exception that push error code. */
	.macro K_IDT_NO_ERROR_CODE number

	.globl	k_idt_int\number
k_idt_int\number:
	pushq	$0
	pushq	$\number	
	jmp	1f

	.endm

K_IDT_NO_ERROR_CODE 0
K_IDT_NO_ERROR_CODE 1
K_IDT_NO_ERROR_CODE 2
K_IDT_NO_ERROR_CODE 3
K_IDT_NO_ERROR_CODE 4
K_IDT_NO_ERROR_CODE 5
K_IDT_NO_ERROR_CODE 6
K_IDT_NO_ERROR_CODE 7
K_IDT_NO_ERROR_CODE 9
K_IDT_NO_ERROR_CODE 15
K_IDT_NO_ERROR_CODE 16
K_IDT_NO_ERROR_CODE 18
K_IDT_NO_ERROR_CODE 19
K_IDT_NO_ERROR_CODE 20
K_IDT_NO_ERROR_CODE 21
K_IDT_NO_ERROR_CODE 22
K_IDT_NO_ERROR_CODE 23
K_IDT_NO_ERROR_CODE 24
K_IDT_NO_ERROR_CODE 25
K_IDT_NO_ERROR_CODE 26
K_IDT_NO_ERROR_CODE 27
K_IDT_NO_ERROR_CODE 28
K_IDT_NO_ERROR_CODE 29
K_IDT_NO_ERROR_CODE 31

/* Push state. */
	.macro	K_PUSH_REGISTERS

	movq	%r15, -0x8(%rsp)
	movq	%r14, -0x10(%rsp)
	movq	%r13, -0x18(%rsp)
	movq	%r12, -0x20(%rsp)
	movq	%r11, -0x28(%rsp)
	movq	%r10, -0x30(%rsp)
	movq	%r9, -0x38(%rsp)
	movq	%r8, -0x40(%rsp)
	movq	%rdi, -0x48(%rsp)
	movq	%rsi, -0x50(%rsp)
	movq	%rbp, -0x58(%rsp)
	movq	%rsp, -0x60(%rsp)
	movq	%rbx, -0x68(%rsp)
	movq	%rdx, -0x70(%rsp)
	movq	%rcx, -0x78(%rsp)
	movq	%rax, -0x80(%rsp)

	subq	$0x80, %rsp

	.endm

/* Pop state. */
	.macro	K_POP_REGISTERS

	addq	$0x80, %rsp

	movq	-0x8(%rsp), %r15
	movq	-0x10(%rsp), %r14
	movq	-0x18(%rsp), %r13
	movq	-0x20(%rsp), %r12
	movq	-0x28(%rsp), %r11
	movq	-0x30(%rsp), %r10
	movq	-0x38(%rsp), %r9
	movq	-0x40(%rsp), %r8
	movq	-0x48(%rsp), %rdi
	movq	-0x50(%rsp), %rsi
	movq	-0x58(%rsp), %rbp
	#movq	-0x60(%rsp), %rsp
	movq	-0x68(%rsp), %rbx
	movq	-0x70(%rsp), %rdx
	movq	-0x78(%rsp), %rcx
	movq	-0x80(%rsp), %rax

	.endm

/* Executing the interrupt handler. */
1:
	K_PUSH_REGISTERS
	callq	k_int_handler
	K_POP_REGISTERS

	addq	$0x10, %rsp
	iretq

/* IRQs. */
	.macro K_IRQ_HANDLER number vector

	.globl	k_irq_handler\number
k_irq_handler\number:
	pushq	$0x0
	pushq	$\vector
	jmp	2f

	.endm

K_IRQ_HANDLER 0 K_IRQ_MASTER_START
K_IRQ_HANDLER 1 K_IRQ_MASTER_START + 1
K_IRQ_HANDLER 2 K_IRQ_MASTER_START + 2
K_IRQ_HANDLER 3 K_IRQ_MASTER_START + 3
K_IRQ_HANDLER 4 K_IRQ_MASTER_START + 4
K_IRQ_HANDLER 5 K_IRQ_MASTER_START + 5
K_IRQ_HANDLER 6 K_IRQ_MASTER_START + 6
K_IRQ_HANDLER 7 K_IRQ_MASTER_START + 7

K_IRQ_HANDLER 8 K_IRQ_SLAVE_START
K_IRQ_HANDLER 9 K_IRQ_SLAVE_START + 1
K_IRQ_HANDLER 10 K_IRQ_SLAVE_START + 2
K_IRQ_HANDLER 11 K_IRQ_SLAVE_START + 3
K_IRQ_HANDLER 12 K_IRQ_SLAVE_START + 4
K_IRQ_HANDLER 13 K_IRQ_SLAVE_START + 5
K_IRQ_HANDLER 14 K_IRQ_SLAVE_START + 6
K_IRQ_HANDLER 15 K_IRQ_SLAVE_START + 7

2:
	K_PUSH_REGISTERS
	callq	k_irq_handler
	K_POP_REGISTERS

	addq	$0x10, %rsp
	iretq

/* Spurious interrupt. */
	.globl	k_spurious_int
k_spurious_int:
	iretq

/* System call. */
	.globl	k_system_call
k_system_call:
	K_PUSH_REGISTERS
	callq	k_system_call_handler
	K_POP_REGISTERS

	iretq

