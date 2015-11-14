#include <stdio.h>
#include <stdint.h>

int
main()
{
    int8_t i, i_2_c;
    uint8_t u;

    // positive integers:
    printf("\nPositive integers\n");
    i = 1;
    // 2's complement conversion
    // 1. complement in unsigned arithmetic:
    u = ((1 << 8) - i) & 0xff;
    i_2_c = (int8_t) u;
    printf( "integer [%d]\n"
		"complement in unsigned arithmetic [%u]\n"
     		"casted to int again [%d]\n", i, u, i_2_c);

    i = 127;
    // 2's complement conversion
    // 1. complement in unsigned arithmetic:
    u = ((1 << 8) - i) & 0xff;
    i_2_c = (int8_t) u;
    printf( "integer [%d]\n"
		"complement in unsigned arithmetic [%u]\n"
     		"casted to int again [%d]\n", i, u, i_2_c);

    // negative integers:
    printf("\nNegative integers\n");
    i = -1;
    // 2's complement conversion
    // 1. complement in unsigned arithmetic:
    u = ((1 << 8) - i) & 0xff; 	// MSB in u is 0 so u treated as unsigned
				// or signed value is the same
    i_2_c = (int8_t) u;
    printf( "integer [%d]\n"
		"complement in unsigned arithmetic [%u]\n"
     		"casted to int again [%d]\n", i, u, i_2_c);

    // this will be error, because highiest positive value positive
    // that can be in expressed in signed 8bit is 127
    i = -128;
    // 2's complement conversion
    // 1. complement in unsigned arithmetic:
    u = ((1 << 8) - i) & 0xff; 	// MSB in u is 1, will OVERFLOW ! 
    i_2_c = (int8_t) u;
    printf( "integer [%d]\n"
		"complement in unsigned arithmetic [%u]\n"
     		"casted to int again [%d], ERROR !\n", i, u, i_2_c);

    i = -127;
    // 2's complement conversion
    // 1. complement in unsigned arithmetic:
    u = ((1 << 8) - i) & 0xff; 	// MSB in u is 0 so u treated as unsigned
				// or signed value is the same
    i_2_c = (int8_t) u;
    printf( "integer [%d]\n"
		"complement in unsigned arithmetic [%u]\n"
     		"casted to int again [%d]\n", i, u, i_2_c);
    return 0;
}
