#include "include/x86_64/paging.h"
#include "include/cpu.h"
#include "include/gdt.h"
#include "include/cr0.h"
#include "include/cr4.h"
#include "include/msr.h"

	.text

	.code32
# Only at page directory & table offsets (no PDPE & PML4E)
# %eax - Memory start
# %ecx - Length
	.globl	k_paging_early_reserve
k_paging_early_reserve:
	andl	$~0xfff, %eax

	addl	$0xfff, %ecx
	andl	$~0xfff, %ecx

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
	andl	$~0xfff, %esi
2:
	movl	%eax, %ecx
	shr	$12, %ecx
	andl	$0x1ff, %ecx

	shl	$3, %ecx
	addl	%ecx, %esi

	movl	%eax, %ebx
	orl	$(K_PTE_G | K_PTE_RW | K_PTE_P), %ebx
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
	subl	$K_PAGE_TABLE_SIZE, %edi

	leal	(K_PAGE_TABLE_SIZE | K_PML4E_RW | K_PML4E_P)(%ebx), %edx
	movl	%edx, (%edi)

	# PDPE
	leal	K_PAGE_TABLE_SIZE(%ebx), %edi
	movl	$(K_PAGE_TABLE_SIZE / 4), %ecx
	rep stosl
	subl	$K_PAGE_TABLE_SIZE, %edi

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

# Map 0xffffffff80000000 to existing page directories & tables.
# PML4E - 511
# PDPE - 510
# Thanks to sharing same root page directories, subsequent mappings
# will be created but won't be used.
	.globl	k_paging_identity_map
k_paging_identity_map:
	# PML4E
	movl	K_PHYSICAL_ADDRESS(k_page_table), %eax
	leal	(K_PAGE_TABLE_SIZE | K_PML4E_RW | K_PML4E_P)(%eax), %ebx
	movl	%ebx, (8 * 511)(%eax)

	# PDPE
	addl	$K_PAGE_TABLE_SIZE, %eax
	leal	(K_PAGE_TABLE_SIZE | K_PDPE_RW | K_PDPE_P)(%eax), %ebx
	movl	%ebx, (8 * 510)(%eax)

	ret

	.globl	k_paging_init
k_paging_init:
	movl	%cr4, %eax
	orl	$K_CR4_PAE, %eax
	movl	%eax, %cr4

	movl	K_PHYSICAL_ADDRESS(k_page_table), %eax
	movl	%eax, %cr3

	movl	$K_MSR_IA32_EFER, %ecx
	rdmsr
	orl	$(K_MSR_IA32_EFER_LME | K_MSR_IA32_EFER_NXE), %eax
	wrmsr

	movl	$(K_CR0_PG | K_CR0_PE), %eax
	movl	%eax, %cr0

	ret

	.globl	k_memory_mode_init
k_memory_mode_init:
	lgdt	K_PHYSICAL_ADDRESS(first_gdt_desc)
	movl	$K_GDT_KERNEL_DATA_SELECTOR, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	jmp	$K_GDT_KERNEL_CODE_SELECTOR, $K_PHYSICAL_ADDRESS(k_memory_mode_64bit)

	.code64

k_paging_remove_identity_map:
	# PML4E
	movq	k_page_table, %rax
	movq	$0, (%rax)

	# PDPE
	addq	$K_PAGE_TABLE_SIZE, %rax
	movq	$0, (%rax)

	# Flush TLB
	movq	%cr3, %rax
	movq	%rax, %cr3

	ret

k_memory_mode_64bit:
	leaq	1f, %rax
	jmp	*%rax
1:

	lgdt	gdt_desc

	lidt	first_idt

	pushq	$0
	popfq

	addq	$K_IMAGE_BASE, %rsp

	addq	$K_IMAGE_BASE, k_page_table
	callq	k_paging_remove_identity_map

	callq	k_main

1:
	jmp	1b

	.data
#include "include/gdt-descriptor.S"
	.globl	first_gdt
	.p2align 3
first_gdt:
	K_GDT_ENTRY g=0
	K_GDT_ENTRY base=0x0 limit=0x0 type=0xa s=1 dpl=0 p=1 l=1 g=1
	K_GDT_ENTRY base=0x0 limit=0x0 type=0x2 s=1 dpl=0 p=1
first_gdt_end:

	.p2align 3
first_gdt_desc:
	.word	first_gdt_end-first_gdt-1
	.long	K_PHYSICAL_ADDRESS(first_gdt)

	.p2align 3
gdt_desc:
	.word	first_gdt_end-first_gdt-1
	.quad	first_gdt

	.p2align 3
first_idt:
	.word	0
	.quad	0

