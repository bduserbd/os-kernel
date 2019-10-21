	.text
	.globl	k_task_arch_set_new_context
k_task_arch_set_new_context:
	pusha

	movl	36(%esp), %eax
	movl	$restore, (%eax)

	movl	40(%esp), %eax
	movl	%esp, (%eax)

	movl	44(%esp), %ebx
	movl	48(%esp), %esp

	//call	k_task_spin_unlock
	jmp	*%ebx

restore:
	popa
	ret

	## Save old context
	#pusha
	#movl	%esp, %eax
	#addl	$(32 + 4), %eax
	#movl	(%eax), %eax

	#testl	%eax, %eax
	#jz	new_context

	#movl	%esp, 0x10(%eax)

#new_context:
	## Set new context
	#movl	%esp, %eax
	#addl	$(32 + 8), %eax
	#movl	(%eax), %eax
	#movl	0x10(%eax), %esp
	#popa

	#ret

