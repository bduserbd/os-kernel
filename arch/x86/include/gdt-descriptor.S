# GDT entry.
        .macro  K_GDT_ENTRY base=0 limit=0 type=0 s=0 p=0 db=0 g=0

        .word   (\limit & 0xffff)
        .word   (\base & 0xffff)
        .byte   ((\base >> 16) & 0xff)
        .byte   (\p << 7) | (\s << 4) | (\type)
        .byte   (\g << 7) | (\db << 6) | ((\limit >> 16) & 0xf)
        .byte   ((\base >> 24) & 0xff)

        .endm

