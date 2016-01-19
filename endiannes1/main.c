#include <stdio.h>
#include <stdint.h>

int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } e = { 0x01000000 };

    return e.c[0];
}

union a {
	char c;
	int i;
	float f;
} temp;

int main(void)
{
    printf("System is %s-endian.\n",
        is_big_endian() ? "big" : "little");
    temp.c = 'A';
    temp. i = 77;
    printf("[%c]\n", temp.c);

    return 0;
}
