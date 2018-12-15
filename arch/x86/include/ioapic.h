#ifndef K_IOAPIC_H
#define K_IOAPIC_H

/* Accessing I/O APIC registers. */
enum {
	K_IOAPIC_REGSEL	= 0x00,
	K_IOAPIC_WIN	= 0x10,
};

/* I/O APIC registers. */
enum {
	K_IOAPIC_ID	= 0x00,
	K_IOAPIC_VER	= 0x01,
	K_IOAPIC_ARB	= 0x02,
	K_IOAPIC_REDTBL	= 0x10,
};

/* I/O APIC version register. */
#define K_IOAPIC_VER_MAX_REDIRECT_ENTRIES(ver)	(((ver >> 16) & 0xff) + 1)

/* I/O APIC redirection table. */
#define K_IOAPIC_REDTBL_INTVEC(intvec)	((intvec) & 0xff)

#define K_IOAPIC_REDTBL_DELMOD_FIXED		(0 << 8)
#define K_IOAPIC_REDTBL_DELMOD_LOWEST_PRIORITY	(1 << 8)

#define K_IOAPIC_REDTBL_DESTMOD_PHYSICAL	(0 << 11)
#define K_IOAPIC_REDTBL_DESTMOD_LOGICAL		(1 << 11)

#define K_IOAPIC_REDTBL_DELIVS_IDLE	(0 << 12)
#define K_IOAPIC_REDTBL_DELIVS_PENDING	(1 << 12)

#define K_IOAPIC_REDTBL_TRIGGER_EDGE	(0 << 15)
#define K_IOAPIC_REDTBL_TRIGGER_LEVEL	(1 << 15)

#define K_IOAPIC_REDTBL_INTERRUPT_MASK	(1 << 16)

void k_ioapic_init(void);

#endif

