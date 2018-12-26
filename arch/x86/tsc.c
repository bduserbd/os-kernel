#include "include/tsc.h"
#include "include/8253.h"
#include "kernel/include/time/time.h"
#include "kernel/include/div64.h"
#include "kernel/include/video/print.h"

k_uint64_t k_tsc_frequency = 0;

void k_tsc_sleep(unsigned int milliseconds)
{
	k_uint64_t end;
	k_uint64_t ticks;

	if (!milliseconds || !k_tsc_frequency)
		return;

#ifdef K_BITS_32
	k_div64(k_tsc_frequency, K_MILLISECONDS_PER_SECOND, &ticks, NULL);
#elif K_BITS_64
	ticks = k_tsc_frequency / K_MILLISECONDS_PER_SECOND;
#endif

	end = k_rdtsc() + milliseconds * ticks;

	while (k_rdtsc() < end) ;
}

void k_tsc_init(void)
{
	int i;
	k_uint64_t a, b, sum;
	k_uint64_t frequency;

	sum = 0;
	b = k_rdtsc();

#define SLEEP	10
#define TIMES	10
#define TOTAL	(SLEEP * TIMES)

	for (i = 0; i < TIMES; i++) {
		a = b; 

		k_pit_channel2_sleep(SLEEP);

		b = k_rdtsc();

		sum += b - a;
	}

#ifdef K_BITS_32
	k_div64(sum, TOTAL, &frequency, NULL);
#elif K_BITS_64
	frequency = sum / TOTAL;
#endif

	k_tsc_frequency = frequency * K_MILLISECONDS_PER_SECOND;
	k_printf("CPU speed :%lluHz\n", k_tsc_frequency);
}

