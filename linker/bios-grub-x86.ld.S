ENTRY(_start)

SECTIONS {
	. = K_IMAGE_BASE + K_IMAGE_OFFSET;

	__k_start = .;

#ifdef K_CONFIG_BIOS
	.multiboot : AT (ADDR(.multiboot) - K_IMAGE_BASE)
	{
		*(.multiboot)
	}
#elif K_CONFIG_UEFI
	.multiboot2 : AT (ADDR(.multiboot2) - K_IMAGE_BASE)
	{
		*(.multiboot2)
	}
#endif

	.text : AT (ADDR(.text) - K_IMAGE_BASE)
	{
		*(.text)
	}

	.rodata : AT (ADDR(.rodata) - K_IMAGE_BASE)
	{
		*(.rodata)
	}

	.data : AT (ADDR(.data) - K_IMAGE_BASE)
	{
		*(.data)
	}

	.bss : AT (ADDR(.bss) - K_IMAGE_BASE)
	{
		*(.bss)
	}

	.k_strtab : AT (ADDR(.k_strtab) - K_IMAGE_BASE)
	{
		*(.k_strtab)
	}

	.k_symtab : AT (ADDR(.k_symtab) - K_IMAGE_BASE)
	{
		__k_symtab_start = .;
		*(.k_symtab)
		__k_symtab_end = .;
	}

	.user : AT (ADDR(.user) - K_IMAGE_BASE)
	{
		__k_user_start = .;
		*(.user)
		__k_user_end = .;
	}

#ifdef K_CONFIG_SMP
	__k_ap_start = .;
	.ap_start : AT (ADDR(.ap_start) - K_IMAGE_BASE)
	{
		*(.ap_start)
	}
	__k_ap_end = .;
#endif

	__k_stack_end = .;
	.kernel_stack : AT (ADDR(.kernel_stack) - K_IMAGE_BASE)
	{
		*(.kernel_stack)
	}
	__k_stack_start = .;

	__k_end = .;
}

