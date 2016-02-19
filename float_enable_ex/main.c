/*
 * @file    main.c
 * @brief   Enable and generate floating point exceptions.
 * @author  Piotr Gregor <piotrek.gregor at gmail.com>
 * @date    19 Feb 2016
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
    /* Enable some exceptions.  At startup all
     * exceptions are masked.  */
    /* The following macros are defined iff
     * the implementation supports this kind of exception:
     * FE_INEXACT		inexact result
     * FE_DIVBYZERO		division by zero
     * FE_UNDERFLOW		result not representable due to underflow
     * FE_OVERFLOW		result not representable due to overflow
     * FE_INVALID		invalid operation
     *
     * FE_ALL_EXCEPT	    bitwise OR of all supported exceptions */
    feenableexcept (FE_ALL_EXCEPT);

   /* The next macros are defined iff
    * the appropriate rounding mode is supported
    * by the implementation:
    * FE_TONEAREST		round to nearest
    * FE_UPWARD		round toward +Inf
    * FE_DOWNWARD		round toward -Inf
    * FE_TOWARDZERO	round toward 0 */
    feenableexcept (FE_TONEAREST);
}

int
main(void)
{
    float f;
    f = 0.0/0.0;
    return 0;
}
