#ifndef K_E1000_H
#define K_E1000_H

/* E1000 registers. */
enum {
	K_E1000_CTRL	= 0x00,
	K_E1000_STATUS	= 0x08,
	K_E1000_EERD	= 0x14,
};

/* E1000 EEPROM Read Register. */
enum {
	K_E1000_EERD_START	= (1 << 0),
	K_E1000_EERD_DONE	= (1 << 4),
};

#define K_E1000_EERD_ADDR(addr)	((addr & 0xff) << 8)

#define K_E1000_EERD_DATA(eerd)	((eerd >> 16) & 0xffff)

/* E1000 EEPROM registers. */
enum {
	K_E1000_ETHERNET_ADDRESS0	= 0x0,
	K_E1000_ETHERNET_ADDRESS1	= 0x1,
	K_E1000_ETHERNET_ADDRESS2	= 0x2,
	K_E1000_EEPROM_INIT_CONTROL1	= 0xa,
};

/* E1000 EEPROM Initialization Control Word 1. */
#define K_E1000_EEPROM_INIT_CONTROL1_SIGNATURE_VALID(icw1)	\
	(((icw1 >> 14) & 0x3) == 0b01)

#endif

