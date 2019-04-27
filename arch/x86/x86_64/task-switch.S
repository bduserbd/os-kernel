#include "include/x86_64/task-state.S"

	.text
	.globl	k_task_arch_set_new_context
k_task_arch_set_new_context:
	# Save old context
	K_PUSH_REGISTERS
	movq	%rsp, 0x20(%rdi)

	# Set new context
	movq	0x20(%rsi), %rsp
	K_POP_REGISTERS

	retq

