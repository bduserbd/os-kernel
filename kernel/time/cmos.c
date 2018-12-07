#include "kernel/include/time/cmos.h"
#include "kernel/include/io/io.h"
#include "kernel/include/video/print.h"
#include "kernel/include/bcd.h"

static inline k_uint8_t k_cmos_read(k_uint8_t address)
{
	k_outb(address, K_CMOS_ADDRESS);
	return k_inb(K_CMOS_DATA);
}

static int k_cmos_format(k_uint8_t data)
{
	if (k_cmos_read(K_CMOS_STATUS_REGISTER_B) & K_CMOS_STATUS_REGISTER_B_BINARY)
		return data;
	else
		return k_bcd_from(data);
}

static int k_cmos_read_real(k_uint8_t address)
{
	return k_cmos_format(k_cmos_read(address));
}

static int k_cmos_read_hours(void)
{
	k_uint8_t hours;

	hours = k_cmos_read(K_RTC_HOURS);

	if (k_cmos_read(K_CMOS_STATUS_REGISTER_B) & K_CMOS_STATUS_REGISTER_B_24_HOUR_MODE)
		return k_cmos_format(hours);
	else
		return k_cmos_format(hours) + (hours & (1 << 7)) ? 12 : 0;
}

void k_cmos_init(void)
{
	k_printf("%d/%d/%d-%d:%d:%d\n",
			k_cmos_read_real(K_RTC_DATE_DAY),
			k_cmos_read_real(K_RTC_DATE_MONTH),
			k_cmos_read_real(K_RTC_DATE_YEAR),
			k_cmos_read_hours(),
			k_cmos_read_real(K_RTC_MINUTES),
			k_cmos_read_real(K_RTC_SECONDS)
	);
}

