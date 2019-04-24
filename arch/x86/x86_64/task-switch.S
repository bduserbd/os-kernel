	.text
	.globl	k_task_arch_set_new_context
k_task_arch_set_new_context:
	# Save old context
	movq	%r15, 0x90(%rdi)
	movq	%r14, 0x88(%rdi)
	movq	%r13, 0x80(%rdi)
	movq	%r12, 0x78(%rdi)
	movq	%r11, 0x70(%rdi)
	movq	%r10, 0x68(%rdi)
	movq	%r9, 0x60(%rdi)
	movq	%r8, 0x58(%rdi)

	movq	%rsi, 0x38(%rdi)
	movq	%rdi, 0x30(%rdi)
	movq	%rbp, 0x28(%rdi)
	movq	%rsp, 0x20(%rdi)
	movq	%rdx, 0x18(%rdi)
	movq	%rcx, 0x10(%rdi)
	movq	%rbx, 0x8(%rdi)
	movq	%rax, (%rdi)

	popq	0x50(%rdi)

	# Set new context
	movq	0x20(%rsi), %rsp
	pushq	0x50(%rsi)

	movq	0x90(%rsi), %r15
	movq	0x88(%rsi), %r14
	movq	0x80(%rsi), %r13
	movq	0x78(%rsi), %r12
	movq	0x70(%rsi), %r11
	movq	0x68(%rsi), %r10
	movq	0x60(%rsi), %r9
	movq	0x58(%rsi), %r8

	movq	0x30(%rsi), %rdi
	movq	0x28(%rsi), %rbp
	movq	0x18(%rsi), %rdx
	movq	0x10(%rsi), %rcx
	movq	0x8(%rsi), %rbx
	movq	(%rsi), %rax
	movq	0x38(%rsi), %rsi

	ret

