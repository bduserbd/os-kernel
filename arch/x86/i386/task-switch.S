	.text
	.globl	k_task_arch_set_new_context
k_task_arch_set_new_context:
	# Save old context
	pusha
	movl	%esp, %eax
	addl	$(32 + 4), %eax
	movl	(%eax), %eax
	movl	%esp, 0x10(%eax)

	# Set new context
	movl	%esp, %eax
	addl	$(32 + 8), %eax
	movl	(%eax), %eax
	movl	0x10(%eax), %esp
	popa

	ret

