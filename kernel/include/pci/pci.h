#ifndef K_PCI_H
#define K_PCI_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/pci/pci-ids.h"
#include "kernel/include/io/io.h"

/*
 * PCI configuration space:
 * ------------------------
 *
 * 31         24 23        16 15         8 7           0
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |        Device ID        |        Vendor ID        | 00h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |         Status          |         Command         | 04h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |              Class Code              | Revision ID| 08h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |    BIST    | Header Type| Lat. Timer |Cacheline Sz| 0Ch
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                       BAR # 0                     | 10h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                       BAR # 1                     | 14h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                       BAR # 2                     | 18h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                       BAR # 3                     | 1Ch
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                       BAR # 4                     | 20h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                       BAR # 5                     | 24h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                Cardbus CIS Pointer                | 28h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |       Subsystem ID      |   Subsystem Vendor ID   | 2Ch
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |            Expansion ROM Base Address             | 30h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |               Reserved               |Cap. Pointer| 34h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |                     Reserved                      | 38h
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 * |  Max Lat.  |  Min Gnt.  |  Int. Pin  |  IRQ. Line | 3Ch
 * |++++++++++++|++++++++++++|++++++++++++|++++++++++++|
 */

/* Offsets into the configuration space. */
#define K_PCI_CONFIG_REG_VENDOR_ID	0x00
#define K_PCI_CONFIG_REG_DEVICE_ID	0x02

#define K_PCI_CONFIG_REG_COMMAND	0x04
# define K_PCI_CONFIG_REG_COMMAND_IO_SPACE	(1 << 0)
# define K_PCI_CONFIG_REG_COMMAND_MEMORY_SPACE	(1 << 1)
# define K_PCI_CONFIG_REG_COMMAND_BUS_MASTER	(1 << 2)

#define K_PCI_CONFIG_REG_STATUS		0x06
#define K_PCI_CONFIG_REG_REVISION_ID	0x08

#define K_PCI_CONFIG_REG_CLASS_CODE		0x09
# define K_PCI_CONFIG_REG_PROG_IF		0x09
# define K_PCI_CONFIG_REG_SUBCLASS		0x0a
# define K_PCI_CONFIG_REG_BASECLASS		0x0b

#define K_PCI_CONFIG_REG_CACHELINE_SIZE	0x0c
#define K_PCI_CONFIG_REG_LATENCY_TIMER	0x0d
#define K_PCI_CONFIG_REG_HEADER_TYPE	0x0e
#define K_PCI_CONFIG_REG_BIST		0x0f
#define K_PCI_CONFIG_REG_IRQ		0x3c

/* Base Address - BAR. */
#define K_PCI_CONFIG_REG_BAR0	0x10
#define K_PCI_CONFIG_REG_BAR1	0x14
#define K_PCI_CONFIG_REG_BAR2	0x18
#define K_PCI_CONFIG_REG_BAR3	0x1c
#define K_PCI_CONFIG_REG_BAR4	0x20
#define K_PCI_CONFIG_REG_BAR5	0x24

/* PCI Structure */
#define K_PCI_NBUSES		256
#define K_PCI_NDEVICES		32
#define K_PCI_NFUNCTIONS	8

/* Address & Data I/O access ports */
#define K_PCI_CONFIG_ADDRESS	0xcf8
#define K_PCI_CONFIG_DATA		0xcfc

/*
 * PCI configuration address register:
 * -----------------------------------
 *
 * |++++++|+++++++++|+++++|++++++|++++++++|++++++++|++++|
 * |  31  | 30 - 24 |23-16| 15-11|  10-8  |  7 - 2 | 1-0|
 * |++++++|+++++++++|+++++|++++++|++++++++|++++++++|++++|
 * |Enable| Reserved| Bus |Device|Function|Register|Zero|
 * |++++++|+++++++++|+++++|++++++|++++++++|++++++++|++++|
 *
 */
#define K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg)	\
	((1 << 31) | (bus << 16) | (dev << 11) | (func << 8) | reg)

/* Read & write PCI configuration address registers */ 
static inline k_uint8_t k_pci_read_config_byte(k_uint8_t bus, k_uint8_t dev, k_uint8_t func,
		k_uint8_t reg)
{
	k_outl(K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg), K_PCI_CONFIG_ADDRESS);

	return k_inb(K_PCI_CONFIG_DATA + (reg & 3));
}

static inline k_uint16_t k_pci_read_config_word(k_uint8_t bus, k_uint8_t dev, k_uint8_t func,
		k_uint8_t reg)
{
	k_outl(K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg), K_PCI_CONFIG_ADDRESS);

	return k_inw(K_PCI_CONFIG_DATA + (reg & 2));
}

static inline k_uint32_t k_pci_read_config_long(k_uint8_t bus, k_uint8_t dev, k_uint8_t func,
		k_uint8_t reg)
{
	k_outl(K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg), K_PCI_CONFIG_ADDRESS);

	return k_inl(K_PCI_CONFIG_DATA);
}

static inline void k_pci_write_config_byte(k_uint8_t bus, k_uint8_t dev, k_uint8_t func,
		k_uint8_t reg, k_uint8_t data)
{
	k_outl(K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg), K_PCI_CONFIG_ADDRESS);
	k_outb(data, K_PCI_CONFIG_DATA);
}

static inline void k_pci_write_config_word(k_uint8_t bus, k_uint8_t dev, k_uint8_t func,
		k_uint8_t reg, k_uint16_t data)
{
	k_outl(K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg), K_PCI_CONFIG_ADDRESS);
	k_outw(data, K_PCI_CONFIG_DATA);
}

static inline void k_pci_write_config_long(k_uint8_t bus, k_uint8_t dev, k_uint8_t func,
		k_uint8_t reg, k_uint32_t data)
{
	k_outl(K_PCI_MAKE_CONFIG_ADDRESS(bus, dev, func, reg), K_PCI_CONFIG_ADDRESS);
	k_outl(data, K_PCI_CONFIG_DATA);
}

struct k_pci_index {
	k_uint32_t bus;
	k_uint32_t dev;
	k_uint32_t func;
};

typedef k_error_t (*pci_device_hook_t)(struct k_pci_index);

void k_pci_iterate_devices(pci_device_hook_t);
k_error_t k_pci_check_device_class(struct k_pci_index, int, int, int);

void k_pci_set_bus_mastering(struct k_pci_index);

#endif

