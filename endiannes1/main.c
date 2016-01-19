#include <stdio.h>
#include <stdint.h>

int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } e = { 0x01000000 };
	char *cp = (char *)&e;
    printf("e.c[0]: [%d][%d]\n", e.c[0], *(cp++));
    printf("e.c[1]: [%d][%d]\n", e.c[1], *(cp++));
    printf("e.c[2]: [%d][%d]\n", e.c[2], *(cp++));
    printf("e.c[3]: [%d][%d]\n", e.c[3], *cp);
    return e.c[0];
}

union a {
	char c;
	int i;
	float f;
} temp;

int main(void)
{
	int res;
	res = is_big_endian();
    	printf("System is %s-endian.\n",
        	res ? "big" : "little");
    	temp.c = 'A';
    	temp. i = 77;
    	printf("[%c]\n", temp.c);

    	return 0;
}
