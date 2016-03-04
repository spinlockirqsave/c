/*
 * @file    main.c
 * @author  Piotr Gregor <piotrek.gregor gmail com>
 * @brief   Bernoulli distribution.
 * @details Calls random device kernel module to get
 *          sample of random bits and draws a Bernoulli
 *          distribution from it.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h> /* O_RDONLY */


#define BITS_PER_BYTE 0x8u
#define BIT_0 0x1u
#define BIT_1 0x2u
#define BIT_2 0x4u
#define BIT_3 0x8u
#define BIT_4 0x10u
#define BIT_5 0x20u
#define BIT_6 0x40u
#define BIT_7 0x80u

#define URANDOM_DEVICE "/dev/urandom"
#define RANDOM_DEVICE "/dev/random"

/*
 * @brief   Read @outlen bytes from random device
 *          to array @out.
 */
int
get_random_samples(char *out, size_t outlen)
{
    size_t read_n;
    ssize_t res;

    int fd = open(URANDOM_DEVICE, O_RDONLY);
    if (fd == -1)
        return -1;
    read_n = 0;
    while (read_n < outlen)
    {
       res = read(fd, out + read_n, outlen - read_n);
       if (res < 0)
       {
           // error, unable to read /dev/random
           close(fd);
           return -2;
       }
       read_n += res;
    }
    return 0;
}

/*
 * @brief   Draw vector of Bernoulli samples.
 * @details @x and @resolution determines probability
 *          of success in Bernoulli distribution
 *          and accuracy of results: p = x/resolution.
 * @param   resolution: number of segments in one byte
 *          of random data used to draw one sample in output array
 * @param   x: determines used probability
 * @param   n: number of samples in result vector
 */
int
get_bernoulli_samples(char *out, uint32_t n, uint32_t resolution, uint32_t x)
{
    int res;
    size_t bytes_n, i;
    char *rnd_bits, uniform_byte;

    if (out == NULL || n == 0 || resolution == 0 || x > resolution)
        return -1;
    /* how many bytes per sample we neeed, rounded up */
    bytes_n = n;
    rnd_bits = malloc(bytes_n);
    if (rnd_bits == NULL)
        return -2;
    res = get_random_samples(rnd_bits, bytes_n);
    if (res < 0)
    {
        free(rnd_bits);
        return -3;
    }

    i = 0;
    while (i < n)
    {
        /* get Bernoulli sample */
        /* read byte */
        uniform_byte = rnd_bits[i];
        /* decision */
        if (uniform_byte / (256 / resolution) < x)
            out[i] = 1;
        else
            out[i] = 0;
        ++i;
    }

    free(rnd_bits);    
    return 0;
}

void
print_c(char *c, int n)
{
    int i = 0;
    while (i < n)
    {
        printf( "[%d]", (int)c[i]);
        ++i;
    }
    printf("\n");
}

int
main(void)
{
    int res;
    char c[256];

    res = get_bernoulli_samples(c, sizeof(c), 256, 1); /* 1/256 = 0.0039 */
    if (res < 0) return -1;
    print_c(c, 256);

    res = get_bernoulli_samples(c, sizeof(c), 256, 2); /* 2/256 = 0.0078 */ 
    if (res < 0) return -1;
    print_c(c, 256);

    res = get_bernoulli_samples(c, sizeof(c), 256, 13); /* 13/256 = 0.0508 */
    if (res < 0) return -1;
    print_c(c, 256);

    res = get_bernoulli_samples(c, sizeof(c), 256, 128); /* 128/256 = 0.5 */
    if (res < 0) return -1;
    print_c(c, 256);

    res = get_bernoulli_samples(c, sizeof(c), 256, 200); /* 200/256 = 0.7813 */
    if (res < 0) return -1;
    print_c(c, 256);

    return 0;
}
