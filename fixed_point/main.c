#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef int16_t Q7_8;

Q7_8
q7_8_add(Q7_8 a, Q7_8 b)
{
    int32_t tmp;
    tmp = a + b;
    /* saturation */
    if (tmp > INT16_MAX)
        tmp = INT16_MAX;
    else if (tmp < INT16_MIN)
        tmp = INT16_MIN;

    return (Q7_8) tmp;
};

int
main()
{
    return 0;
}
