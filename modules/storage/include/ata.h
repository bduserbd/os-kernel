#ifndef K_ATA_H
#define K_ATA_H

#include "kernel/include/video/print.h"

/* ATA identification. */
struct k_ata_identification {
	__u16   flags;			/* Word 0 */
	__u16   unused0[9];
	__u8    serial_number[20];	/* Word 10-19*/
	__u16   unused1[3];
	__u8    firmware_revision[8];	/* Word 23-26 */
	__u8    model_number[40];	/* Word 27-46 */
	__u16   unused2[2];
	__u32   capabilities;		/* Word 49-50 */
	__u16   unused3[9];
	__u32   sectors_lba28;		/* Word 60-61 */
	__u16   unused4[38];
	__u64   sectors_lba48;		/* Word 100-103 */
	__u16   unused5[152];
} __attribute__((packed));

static inline void k_ata_print_string(k_uint8_t *str, int length)
{
	int i;
	char s[3];

	s[2] = 0;

	for (i = 0; i < length / 2; i += 2) {
		s[0] = str[i + 1];
		s[1] = str[i];
		k_printf("%s", s);
	}
}

#endif

