#include "include/div64.h"

void k_div64(k_uint64_t n, k_uint64_t d, k_uint64_t *q, k_uint64_t *r)
{
	int i;
	k_uint64_t _q = 0, _r = 0;

	for (i = 0; i < 64; i++) {
		_r <<= 1;

		if (n & (1ULL << 63))
			_r |= 1;

		_q <<= 1;
		n <<= 1;

		if (_r >= d) {
			_q |= 1;
			_r -= d;
		}
	}

	if (q)
		*q = _q;

	if (r)
		*r = _r;
}

