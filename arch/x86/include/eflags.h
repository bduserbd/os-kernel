#ifndef K_EFLAGS_H
#define K_EFLAGS_H

#define K_EFLAGS_CF	(1 << 0)
#define K_EFLAGS_FIXED	(1 << 1)
#define K_EFLAGS_PF	(1 << 2)
#define K_EFLAGS_AF	(1 << 4)
#define K_EFLAGS_ZF	(1 << 6)
#define K_EFLAGS_SF	(1 << 7)
#define K_EFLAGS_TF	(1 << 8)
#define K_EFLAGS_IF	(1 << 9)
#define K_EFLAGS_DF	(1 << 10)
#define K_EFLAGS_OF	(1 << 11)
#define K_EFLAGS_IOPL	(0x3 << 12)
#define K_EFLAGS_NT	(1 << 14)
#define K_EFLAGS_RF	(1 << 16)
#define K_EFLAGS_VM	(1 << 17)
#define K_EFLAGS_AC	(1 << 18)
#define K_EFLAGS_VIF	(1 << 19)
#define K_EFLAGS_VIP	(1 << 20)
#define K_EFLAGS_ID	(1 << 21)

#endif

