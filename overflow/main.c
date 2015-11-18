#include <stdio.h>
#include <stdint.h>
#include <complex.h>
#include <stdlib.h>


// assign if it fits
#define assign(dest, src) ({ \
	typeof(src) __x = (src); \
	typeof(dest) __y = __x; \
	(__x == __y && ((__x < 1) == (__y < 1)) ? \
	(void)((dest) = __y), 0 : 1); \
})

// add positive integers if it won't overflow
#define __add_of(c, a, b) ({ \
	typeof(a) __a = a; \
	typeof(b) __b = b; \
	((__MAX(typeof(c)) - (__b) >= (__a)) ? \
	assign(c, __a + __b) : 1); \
})


// add any configuration of integers (s/s, s/u, , u/s, u/u)
// if it won't overflow
#define add_of(c, a, b) ({ \
	typeof(a) __a = a; \
	typeof(b) __b = b; \
	(__b) < 1 ? \
	((__MIN(typeof(c)) - (__b) <= (__a))? \
	assign(c, __a + __b) : 1) : \
	((__MAX(typeof(c)) - (__b) >= (__a)) ? \
	assign(c, __a + __b) : 1); \
})

// subtruct any configuration of integers (s/s, s/u, , u/s, u/u)
// if it won't overflow
#define sub_of(c, a, b) ({ \
	typeof(a) __a = a; \
	typeof(b) __b = b; \
	(__b) < 1 ? \
	((__MAX(typeof(c)) + (__b) >= (__a)) ? \
	assign(c, __a -__b) : 1) : \
	((__MIN(typeof(c)) + (__b) <= (__a)) ? \
	assign(c, __a - __b) : 1); \
})


int
main()
{
	int8_t x = 1;		// -128 min, 127 max
	uint8_t y = 128;	// [0,255] 
	if (assign(x, y) == 0)
		printf("OK: x [%d], y [%u]\n", x, y);
	else
		printf("NOT OK: x [%d], y [%u]\n", x, y); // NOT OK printed
	x = 1;
	int16_t z = -129;
	if (assign(x, z) == 0)
		printf("OK: x [%d], z [%u]\n", x, z);
	else
		printf("NOT OK: x [%d], z [%u]\n", x, z); // NOT OK printed
    return 0;
}
