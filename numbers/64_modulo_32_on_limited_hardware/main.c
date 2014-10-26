/*
 * http://stackoverflow.com/questions/26558033/how-can-i-implement-a-modulo-operation-on-unsigned-ints-with-limited-hardware-in
 */

#include <stdio.h>
#include <stdint.h>

#define MSBit 0x80000000L

uint32_t mod32(uint32_t a1 /* MSHalf */, uint32_t a2 /* LSHalf */, uint32_t b) {
  uint32_t a = 0;
  for (int i = 31+32; i >= 0; i--) {
    if (a & MSBit) {  // Note 1
        a <<= 1;
        a -= b;
    } else {
        a <<= 1;
    }

    if (a1 & MSBit)
        a++;

    a1 <<= 1;           /* alwas shift a1 */
    if (a2 & MSBit)
        a1++;

    a2 <<= 1;           /* always shift a2 */
    if (a >= b)
        a -= b;
  }
  
  return a;
}

int main() {
    int myIntA1;
    int result = scanf("%d", &myIntA1);

    if (result == EOF) {
        /* ... you're not going to get any input ... */
        printf( "oh dear");
        return -1;
    }
    if (result == 0) {
        while (fgetc(stdin) != '\n') // Read until a newline is found
            ;
    }
    /* OK */
    int myIntA2;
    result = scanf("%d", &myIntA2);

    if (result == EOF) {
        /* ... you're not going to get any input ... */
        printf( "oh dear");
        return -1;
    }
    if (result == 0) {
        while (fgetc(stdin) != '\n') // Read until a newline is found
            ;
    }
    /* OK */
    int b;
    result = scanf("%d", &b);

    if (result == EOF) {
        /* ... you're not going to get any input ... */
        printf( "oh dear");
        return -1;
    }
    if (result == 0) {
        while (fgetc(stdin) != '\n') // Read until a newline is found
            ;
    }
    /* OK */
    int res = mod32( myIntA1, myIntA2, b);
    printf( "res=%d",res);
}
