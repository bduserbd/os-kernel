/* Push state. */
        .macro  K_PUSH_REGISTERS

        movq    %r15, -0x8(%rsp)
        movq    %r14, -0x10(%rsp)
        movq    %r13, -0x18(%rsp)
        movq    %r12, -0x20(%rsp)
        movq    %r11, -0x28(%rsp)
        movq    %r10, -0x30(%rsp)
        movq    %r9, -0x38(%rsp)
        movq    %r8, -0x40(%rsp)
        movq    %rdi, -0x48(%rsp)
        movq    %rsi, -0x50(%rsp)
        movq    %rbp, -0x58(%rsp)
        movq    %rsp, -0x60(%rsp)
        movq    %rbx, -0x68(%rsp)
        movq    %rdx, -0x70(%rsp)
        movq    %rcx, -0x78(%rsp)
        movq    %rax, -0x80(%rsp)

        subq    $0x80, %rsp

        .endm

/* Pop state. */
        .macro  K_POP_REGISTERS

        addq    $0x80, %rsp

        movq    -0x8(%rsp), %r15
        movq    -0x10(%rsp), %r14
        movq    -0x18(%rsp), %r13
        movq    -0x20(%rsp), %r12
        movq    -0x28(%rsp), %r11
        movq    -0x30(%rsp), %r10
        movq    -0x38(%rsp), %r9
        movq    -0x40(%rsp), %r8
        movq    -0x48(%rsp), %rdi
        movq    -0x50(%rsp), %rsi
        movq    -0x58(%rsp), %rbp
        #movq   -0x60(%rsp), %rsp
        movq    -0x68(%rsp), %rbx
        movq    -0x70(%rsp), %rdx
        movq    -0x78(%rsp), %rcx
        movq    -0x80(%rsp), %rax

        .endm

