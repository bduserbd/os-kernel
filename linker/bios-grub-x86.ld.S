ENTRY(_start)

SECTIONS {
	. = 1M;

	__k_start = .;

	.multiboot2 : { *(.multiboot2) }

	.text	: { *(.text) }
	.rodata	: { *(.rodata) }
	.data	: { *(.data) }
	.bss	: { *(.bss) }

	__k_end = .;

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
	}
}

