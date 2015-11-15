#include <stdio.h>
#include <stdint.h>
#include <complex.h>
#include <stdlib.h>

int
main()
{
	int8_t i;
	i = -127;
    	printf( "i: [%d]\n", i);
	i = abs(i);
    	printf( "abs(i): [%d]\n", i);

	i = -128;
    	printf( "i: [%d]\n", i);
	i = abs(i); // this will OVERFLOW
    	printf( "abs(i): [%d]\n", i);
    return 0;
}
