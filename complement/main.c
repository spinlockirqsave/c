#include <stdio.h>
#include <stdint.h>

int
main()
{
    int8_t i, i_2_c;
    uint8_t u;

    // positive integers:
    i = 1;
    // 2's complement conversion
    // 1. complement in unsigned arithmetic:
    u = ((1 << 8) - i) & 0xff;
    i_2_c = (int8_t) u;
    printf( "integer [%d]\n"
		"complement in unsigned arithmetic [%u]\n"
     		"casted to int again [%d]\a\n", i, u, i_2_c);
    return 0;
}
