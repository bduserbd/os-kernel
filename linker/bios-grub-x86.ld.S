ENTRY(_start)

SECTIONS {
	. = 1M;

	__k_start = .;

	.multiboot2 : { *(.multiboot2) }

	.text	: { *(.text) }
	.rodata	: { *(.rodata) }
	.data	: { *(.data) }
	.bss	: { *(.bss) }

#ifdef K_CONFIG_SMP
	__k_ap_start = .;
	.ap_start : { *(.ap_start) }
	__k_ap_end = .;
#endif

	__k_end = .;
}

