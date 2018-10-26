ENTRY(_start)

SECTIONS {
	. = 1M;

	__k_start = .;

#ifdef K_CONFIG_BIOS
	.multiboot : { *(.multiboot) }
#elif K_CONFIG_UEFI
	.multiboot2 : { *(.multiboot2) }
#endif

	.text	: { *(.text) }
	.rodata	: { *(.rodata) }
	.data	: { *(.data) }
	.bss	: { *(.bss) }

	.k_strtab	: { *(.k_strtab) }

	.k_symtab	: {
		__k_symtab_start = .;
		*(.k_symtab)
		__k_symtab_end = .;
	}

#ifdef K_CONFIG_SMP
	__k_ap_start = .;
	.ap_start	: { *(.ap_start) }
	__k_ap_end = .;
#endif

	.kernel_stack	: { *(.kernel_stack) }

	__k_end = .;
}

