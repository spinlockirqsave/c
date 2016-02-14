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


/* Qmn notation
 * real number x is represented by integer X in m+n+1 bits
 * MSB is sign bit, then m bits of integer part
 * and n fractional bits
 * Range that x represents: [-2^(m+n), 2^(m+n) - 1]
 * Range that X can represent: range of x / 2^n = 
 * = [-2^(m+n), 2^(m+n) - 1] / 2^n = [-2^m, 2^m - 1/2^n]
 * Resolution: 1/2^n */

/* Q7.8 type in Qmn notation
 * SMMMMMMM.NNNNNNNN
 * range as normal integer : [-2^15, 2^15 - 1] = [-32768, 32767]
 * range that Q7.8 represents: range of x / 2^n = [-32768, 32767] / 256 =
 * = [-256, 256 - 1/256] = [-256, 255.99609375]
 * Resolution: 1/2^8 = 1/256 = 0.00390625 */
typedef int16_t Q7_8;

/* Addition.
 * Tmp result requires N + 1 bits. */
Q7_8
q7_8_add(Q7_8 a, Q7_8 b)
{
    int32_t tmp; /* tmp result requires 17 bits */
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
    Q7_8 a, b, c;
    a  = 0x01cd;  /* 1.11001101 = 1.80078125 dec (1.8 closest, 1.8 has no better representation in Q7.8) */
    b = 0x0119;   /* 1.00011001 = 1.09765625 dec */
    c = q7_8_add(a, b);
    printf("a[%d] + b[%d] = [%d] (dec), =\n"
            "[%d].[%d] + [%d].[%d] = [%d].[%d] (Q7.8)\n"
            "[%d].[%0.8f] + [%d].[%0.8f] = [%d].[%0.8f] (fractional part divided by 2^n)\n",
          a, b, c,
          a >> 8, a & 0xff, b >> 8, b & 0xff, c >> 8, c & 0xff,
          a >> 8, (float)(a & 0xff)/256, b >> 8, (float)(b & 0xff)/256, c >> 8, (float)(c & 0xff)/256);
    return 0;
}
