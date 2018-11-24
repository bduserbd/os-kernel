	.text
	.globl	_start
_start:
	movl	$0x1, %eax
	xorl	%ebx, %ebx
	int	$0x80

1:
	jmp	1b

