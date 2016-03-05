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


/* if defined use /dev/urandom (will not block),
 * if not defined use /dev/random (may block)*/
#define URANDOM_DEVICE 1

int
log_int(uint32_t x)
{
    int8_t y; /* = log(x) */
    switch (x)
    {
        case 0:
            y = -1;
            break;
        case 1:
            y = 0;
            break;
        case 2:
            y = 1;
            break;
        case 4:
            y = 2;
            break;
        case 8:
            y = 3;
            break;
        case 16:
            y = 4;
            break;
        case 32:
            y = 5;
            break;
        case 64:
            y = 6;
            break;
        case 128:
            y = 7;
            break;
        case 256:
            y = 8;
            break;
        case 512:
            y = 9;
            break;
        case 1024:
            y = 10;
            break;
        case 2048:
            y = 11;
            break;
        case 4096:
            y = 12;
            break;
        case 8192:
            y = 13;
            break;
        case 16384:
            y = 14;
            break;
        case 32768:
            y = 15;
            break;
        case 65536:
            y = 16;
            break;
        case 131072:
            y = 17;
            break;
        case 262144:
            y = 18;
            break;
        case 524288:
            y = 19;
            break;
        case 1048576:
            y = 20;
            break;
        case 2097152:
            y = 21;
            break;
        case 4194304:
            y = 22;
            break;
        case 8388608:
            y = 23;
            break;
        case 16777216:
            y = 24;
            break;
    }
    return y;
}
/*
 * @brief   Read @outlen bytes from random device
 *          to array @out.
 */
int
get_random_samples(unsigned char *out, size_t outlen)
{
    ssize_t res;

#ifdef URANDOM_DEVICE
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
        return -1;
       res = read(fd, out, outlen);
       if (res < 0)
       {
           // error, unable to read /dev/urandom
           close(fd);
           return -2;
       }
#else
    size_t read_n;
    int fd = open("/dev/random", O_RDONLY);
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
           return -3;
       }
       read_n += res;
    }
#endif /* URANDOM_DEVICE */
    close(fd);
    return 0;
}

/*
 * @brief   Draw vector of Bernoulli samples.
 * @details @x and @resolution determines probability
 *          of success in Bernoulli distribution
 *          and accuracy of results: p = x/resolution.
 * @param   resolution: number of segments per sample of output array 
 *          as power of 2: max resolution supported is 2^24=16777216
 * @param   x: determines used probability, x = [0, resolution - 1]
 * @param   n: number of samples in result vector
 */
int
get_bernoulli_samples(char *out, uint32_t n, uint32_t resolution, uint32_t x)
{
    int res;
    size_t i, j;
    uint32_t bytes_per_byte, word;
    unsigned char *rnd_bytes;
    uint32_t uniform_byte;
    uint8_t bits_per_byte;

    if (out == NULL || n == 0 || resolution == 0 || x > (resolution - 1))
        return -1;

    bits_per_byte = log_int(resolution);
    bytes_per_byte = bits_per_byte / BITS_PER_BYTE + 
                        (bits_per_byte % BITS_PER_BYTE ? 1 : 0);
    rnd_bytes = malloc(n * bytes_per_byte);
    if (rnd_bytes == NULL)
        return -2;
    res = get_random_samples(rnd_bytes, n * bytes_per_byte);
    if (res < 0)
    {
        free(rnd_bytes);
        return -3;
    }

    i = 0;
    while (i < n)
    {
        /* get Bernoulli sample */
        /* read byte */
        j = 0;
        word = 0;
        while (j < bytes_per_byte)
        {
            word |= (rnd_bytes[i * bytes_per_byte + j] << (BITS_PER_BYTE * j));
            ++j;
        }
        uniform_byte = word & ((1u << bits_per_byte) - 1);
        /* decision */
        if (uniform_byte < x)
            out[i] = 1;
        else
            out[i] = 0;
        ++i;
    }

    free(rnd_bytes);    
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

    res = get_bernoulli_samples(c, 256, 256, 1); /* 1/256 = 0.0039 */
    if (res < 0) return -1;
    print_c(c, 40);

    res = get_bernoulli_samples(c, sizeof(c), 256, 2); /* 2/256 = 0.0078 */ 
    if (res < 0) return -1;
    print_c(c, 40);

    res = get_bernoulli_samples(c, sizeof(c), 256, 13); /* 13/256 = 0.0508 */
    if (res < 0) return -1;
    print_c(c, 40);

    res = get_bernoulli_samples(c, sizeof(c), 256*256, 328); /* 328/256 = 0.0050 */
    if (res < 0) return -1;
    print_c(c, 40);

    res = get_bernoulli_samples(c, sizeof(c), 256, 200); /* 200/256 = 0.7813 */
    if (res < 0) return -1;
    print_c(c, 40);

    res = get_bernoulli_samples(c, sizeof(c), 256*256, 200); /* 200/256^2 = 0.0031 */
    if (res < 0) return -1;
    print_c(c, 40);

    return 0;
}
