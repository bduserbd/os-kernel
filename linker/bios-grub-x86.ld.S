ENTRY(_start)

SECTIONS {
	. = 1M;

	.multiboot2 : { *(.multiboot2) }

	.text	: { *(.text) }
	.rodata	: { *(.rodata) }
	.data	: { *(.data) }
	.bss	: { *(.bss) }

	__k_end = .;

	. = ALIGN(0x1000);
	__k_first_page_table = .;

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
	}
}

