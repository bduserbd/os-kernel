#ifndef K_PCI_IDS_H
#define K_PCI_IDS_H

#define K_PCI_BASE_CLASS_STORAGE		0x01
# define K_PCI_SUBCLASS_STORAGE_SCSI		0x00
# define K_PCI_SUBCLASS_STORAGE_IDE		0x01
# define K_PCI_SUBCLASS_STORAGE_FLOPPY		0x02
# define K_PCI_SUBCLASS_STORAGE_IPI		0x03
# define K_PCI_SUBCLASS_STORAGE_RAID		0x04
# define K_PCI_SUBCLASS_STORAGE_SATA		0x06
#  define K_PCI_PROG_IF_STORAGE_SATA_AHCI	0x01
# define K_PCI_SUBCLASS_STORAGE_SAS		0x07
# define K_PCI_SUBCLASS_STORAGE_OTHER		0x80

#define K_PCI_BASE_CLASS_NETWORK	0x02
# define K_PCI_SUBCLASS_NETWORK_ETHERNET	0x00
# define K_PCI_SUBCLASS_NETWORK_TOKEN_RING	0x01
# define K_PCI_SUBCLASS_NETWORK_FDDI		0x02
# define K_PCI_SUBCLASS_NETWORK_ATM		0x03
# define K_PCI_SUBCLASS_NETWORK_ISDN		0x04

#define K_PCI_BASE_CLASS_DISPLAY			0x03
# define K_PCI_SUBCLASS_DISPLAY_VGA			0x00
# define K_PCI_SUBCLASS_DISPLAY_XGA			0x01
# define K_PCI_SUBCLASS_DISPLAY_3D			0x02
# define K_PCI_SUBCLASS_DISPLAY_OTHER			0x80

#define K_PCI_BASE_CLASS_MEMORY			0x05
# define K_PCI_SUBCLASS_MEMORY_RAM			0x00
# define K_PCI_SUBCLASS_MEMORY_FLASH			0x01
# define K_PCI_SUBCLASS_MEMORY_OTHER			0x80

#define K_PCI_BASE_CLASS_BRIDGE			0x06
# define K_PCI_SUBCLASS_BRIDGE_HOST			0x00
# define K_PCI_SUBCLASS_BRIDGE_ISA			0x01
# define K_PCI_SUBCLASS_BRIDGE_EISA			0x02
# define K_PCI_SUBCLASS_BRIDGE_MC			0x03
# define K_PCI_SUBCLASS_BRIDGE_PCI			0x04
# define K_PCI_SUBCLASS_BRIDGE_PCMCIA		0x05
# define K_PCI_SUBCLASS_BRIDGE_NUBUS			0x06
# define K_PCI_SUBCLASS_BRIDGE_CARDBUS		0x07
# define K_PCI_SUBCLASS_BRIDGE_RACEWAY		0x08
# define K_PCI_SUBCLASS_BRIDGE_OTHER			0x80

#define K_PCI_BASE_CLASS_COMMUNICATION	0x07
# define K_PCI_SUBCLASS_COMMUNICATION_SERIAL		0x00
# define K_PCI_SUBCLASS_COMMUNICATION_PARALLEL 	0x01
# define K_PCI_SUBCLASS_COMMUNICATION_MULTISERIAL	0x02
# define K_PCI_SUBCLASS_COMMUNICATION_MODEM		0x03
# define K_PCI_SUBCLASS_COMMUNICATION_OTHER		0x80

#define K_PCI_BASE_CLASS_SYSTEM			0x08
# define K_PCI_SUBCLASS_SYSTEM_PIC			0x00
# define K_PCI_SUBCLASS_SYSTEM_PIC_IOAPIC		0x00
# define K_PCI_SUBCLASS_SYSTEM_PIC_IOXAPIC		0x00
# define K_PCI_SUBCLASS_SYSTEM_DMA			0x01
# define K_PCI_SUBCLASS_SYSTEM_TIMER			0x02
# define K_PCI_SUBCLASS_SYSTEM_RTC			0x03
# define K_PCI_SUBCLASS_SYSTEM_K_PCI_HOTPLUG	0x04
# define K_PCI_SUBCLASS_SYSTEM_SDHCI			0x05
# define K_PCI_SUBCLASS_SYSTEM_OTHER			0x80

#define K_PCI_BASE_CLASS_SERIAL			0x0c
# define K_PCI_SUBCLASS_SERIAL_FIREWIRE		0x00
# define K_PCI_SUBCLASS_SERIAL_FIREWIRE_OHCI		0x00
# define K_PCI_SUBCLASS_SERIAL_ACCESS		0x01
# define K_PCI_SUBCLASS_SERIAL_SSA			0x02
# define K_PCI_SUBCLASS_SERIAL_USB			0x03
# define K_PCI_SUBCLASS_SERIAL_USB_UHCI		0x03
#  define K_PCI_PROG_IF_SERIAL_USB_UHCI			0x00
# define K_PCI_SUBCLASS_SERIAL_USB_OHCI		0x03
#  define K_PCI_PROG_IF_SERIAL_USB_OHCI			0x10
# define K_PCI_SUBCLASS_SERIAL_USB_EHCI		0x03
#  define K_PCI_PROG_IF_SERIAL_USB_EHCI			0x20
# define K_PCI_SUBCLASS_SERIAL_USB_XHCI		0x03
#  define K_PCI_PROG_IF_SERIAL_USB_XHCI			0x30
# define K_PCI_SUBCLASS_SERIAL_USB_DEVICE		0x03
# define K_PCI_SUBCLASS_SERIAL_FIBER			0x04
# define K_PCI_SUBCLASS_SERIAL_SMBUS			0x05

#endif

