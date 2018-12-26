	.text
	.globl	k_task_arch_set_new_context
k_task_arch_set_new_context:
	movl	4(%esp), %eax
	
	movl	0x10(%eax), %esp
	pushl	0x2c(%eax)

	movl	0x4(%eax), %ebx
	movl	0x8(%eax), %ecx
	movl	0xc(%eax), %edx

	movl	0x14(%eax), %ebp

	movl	0x18(%eax), %edi
	movl	0x1c(%eax), %esi

	movl	(%eax), %eax

	sti

	ret

