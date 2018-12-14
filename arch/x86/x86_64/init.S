#include "include/x86_64/paging.h"
#include "include/cr4.h"

	.text
	.code32

# Only at page directory & table offsets (no PDPE & PML4E)
# %eax - Memory start
# %ecx - length
	.globl	k_paging_early_reserve
k_paging_early_reserve:
	andl	$0xfffff000, %eax

	addl	$0xfff, %ecx
	andl	$0xfffff000, %ecx

	movl	%eax, %edx
	addl	%ecx, %edx

1:
	movl	%eax, %ecx
	shr	$(12 + 9), %ecx
	andl	$0x1ff, %ecx

	movl	K_PHYSICAL_ADDRESS(k_page_table), %edi
	leal	(2 * K_PAGE_TABLE_SIZE)(%edi), %edi
	shl	$3, %ecx
	addl	%ecx, %edi

	shr	$3, %ecx
	movl	K_PHYSICAL_ADDRESS(k_page_table), %esi
	leal	(3 * K_PAGE_TABLE_SIZE)(%esi), %esi
	shl	$12, %ecx
	addl	%ecx, %esi

	testb	$K_PDE_P, (%edi)
	jnz	2f

	orl	$(K_PDE_RW | K_PDE_P), %esi
	movl	%esi, (%edi)
	andl	$0xfffff000, %esi
2:
	movl	%eax, %ecx
	shr	$12, %ecx
	andl	$0x1ff, %ecx

	shl	$3, %ecx
	addl	%ecx, %esi

	movl	%eax, %ebx
	orl	$(K_PTE_RW | K_PTE_P), %ebx
	movl	%ebx, (%esi)

	addl	$K_PAGE_TABLE_SIZE, %eax
	cmpl	%eax, %edx
	jne	1b

	ret

	.globl	k_paging_table_set_start
k_paging_table_set_start:
	movl	%eax, %ebx
	movl	%ebx, K_PHYSICAL_ADDRESS(k_page_table)

	xorl	%eax, %eax

	# PML4E
	movl	%ebx, %edi
	movl	$(K_PAGE_TABLE_SIZE / 4), %ecx
	rep stosl

	leal	(K_PAGE_TABLE_SIZE | K_PML4E_RW | K_PML4E_P)(%ebx), %edx
	movl	%edx, (%edi)

	# PDPE
	leal	K_PAGE_TABLE_SIZE(%ebx), %edi
	movl	$(K_PAGE_TABLE_SIZE / 4), %ecx
	rep stosl

	leal	(2 * K_PAGE_TABLE_SIZE | K_PML4E_RW | K_PML4E_P)(%ebx), %edx
	movl	%edx, (%edi)

	# Page directory
	leal	(2 * K_PAGE_TABLE_SIZE)(%ebx), %edi
	movl	$(K_PAGE_TABLE_SIZE / 4), %ecx
	rep stosl

	# Page tables
	leal	(3 * K_PAGE_TABLE_SIZE)(%ebx), %edi
	movl	$(K_PAGE_TABLE_SIZE * K_PAGE_TABLE_SIZE / 4), %ecx
	rep stosl

	# Map the page table
	movl	%ebx, %eax
	movl	$K_PAGE_TABLE_TOTAL_SIZE, %ecx
	call	k_paging_early_reserve

	ret

	.globl	k_paging_init
k_paging_init:
	movl	%cr4, %eax
	orl	$K_CR4_PAE, %eax
	movl	%eax, %cr4

	movl	K_PHYSICAL_ADDRESS(k_page_table), %eax
	movl	%eax, %cr3

	ret

