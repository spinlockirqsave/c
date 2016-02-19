/*
 * @file    main.c
 * @brief   Single_producer-multiple_consumers problem
 *          with synchronising of consumers on each row
 *          of the table.
 * @author  Piotr Gregor <piotrek.gregor at gmail.com>
 * @date    18 Feb 2016 2:38 PM
 */


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
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdbool.h>


#define _GNU_SOURCE 1
#include <fenv.h>
static void __attribute__ ((constructor))
trapfpe ()
{
    /* Enable some exceptions.  At startup all exceptions are masked.  */
    feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
}

int
main(void)
{
    float f;
    f = 0.0/0.0;
    return 0;
}
