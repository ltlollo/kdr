// This software is dual-licensed to the public domain and under the following
// license: you are granted a perpetual, irrevocable license to copy, modify,
// publish, and distribute this file as you see fit.

#ifndef PHASH_H
#define PHASH_H

unsigned phash(float, float, unsigned);

#include <stdint.h>

union uf32 {
	float f;
	uint32_t u;
};

float f32sanitize(float f) {
	union uf32 max = { .u = 0x3f7fffff };

	if (f > max.f) {
		return max.f;
	} else if (f < 0.0f) {
		return 0.0f;
	}
	return f;
}

uint32_t u32mix(uint32_t a, uint32_t b) {
	uint32_t res = (a & 0xff000000u) | ((b & 0xff000000u) >> 8) |
		((a & 0x00ff0000u) >> 8) | ((b & 0x00ff0000u) >> 16);
	res = (res & 0xf00ff00fu) |
		((res & 0x0f000f00u) >> 4) | ((res << 4) & 0x0f000f00u);
	res = (res & 0xc3c3c3c3u) |
		((res & 0x30303030u) >> 2) | ((res << 2) & 0x30303030u);
	res = (res & 0x99999999u) |
		((res & 0x44444444u) >> 1) | ((res << 1) & 0x44444444u);
	return res;
}

uint32_t quadrant(uint32_t x) {
	unsigned exp = 0x7e - (x >> 23);
	uint32_t res = (0x80000000u >> exp) | ((x << 9) >> (exp + 1));
	return res;
}

unsigned ctimephash(uint32_t qx, uint32_t qy, unsigned n) {
	uint64_t res = u32mix(qx, qy);

	if (n > 16) {
		n = 16;
	}
	return res >> (32 - 2 * n);
}

unsigned phash(float x, float y, unsigned n) {
	union uf32 nx = { .f = f32sanitize(x) };
	union uf32 ny = { .f = f32sanitize(y) };
	uint32_t qx = quadrant(nx.u);
	uint32_t qy = quadrant(ny.u);
	unsigned res = 0;

	if (n < 4) {
		if (n > 0) {
			res = ((qx >> 30) & 2) | (qy >> 31);
		}
		if (n > 1) {
			res = (res << 2) | ((qx >> 29) & 2) | ((qy >> 30) & 1);
		}
		if (n > 2) {
			res = (res << 2) | ((qx >> 28) & 2) | ((qy >> 29) & 1);
		}
	} else {
		res = ctimephash(qx, qy, n);
	}
	return res;
}

#endif // PHASH_H
