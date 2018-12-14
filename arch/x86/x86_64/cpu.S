#include "include/eflags.h"
#include "include/cpu.h"

	.text
	.code32

k_cpu_eflag:
	pushf

	pushf
	popl	%ebx

	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pushl	%ecx
	popf

	pushf
	popl	%ecx

	popf

	xorl	%ebx, %ecx
	andl	%ecx, %eax

	movl	%eax, %edx
	xorl	%eax, %eax
	testl	%edx, %edx
	setne	%al

	ret

	.globl	k_cpu_is_valid
k_cpu_is_valid:
	movl	$(K_EFLAGS_AC | K_EFLAGS_ID), %eax
	call	k_cpu_eflag
	testl	%eax, %eax
	jz	_exit

	movl	$0x0, %eax
	cpuid
	cmpl	$0x68747541, %ebx
	jnz	_exit
	cmpl	$0x444d4163, %ecx
	jnz	_exit
	cmpl	$0x69746e65, %edx
	jnz	_exit
	cmpl	$0x1, %eax
	jb	_exit

	movl	$0x1, %eax
	cpuid
	andl	$(K_CPUID_PSE | K_CPUID_TSC | K_CPUID_MSR |	\
			K_CPUID_PAE | K_CPUID_APIC | K_CPUID_PGE), %edx
	xorl	$(K_CPUID_PSE | K_CPUID_TSC | K_CPUID_MSR |	\
			K_CPUID_PAE | K_CPUID_APIC | K_CPUID_PGE), %edx
	jnz	_exit

	movl	$0x80000000, %eax
	cpuid
	cmpl	$0x80000001, %eax
	jb	_exit

	movl	$0x80000001, %eax
	cpuid
	andl	$K_CPUID_LM, %edx
	xorl	$K_CPUID_LM, %edx
	jnz	_exit

	xorl	%eax, %eax
	ret

_exit:
	movl	$0x1, %eax
	ret

