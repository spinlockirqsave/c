#include <stdio.h>

#define LTTLE_ENDIAN  0
#define BIG_ENDIAN    1

int main() {
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        printf("LITLE ENDIAN\n");
    else
        printf("BIG ENDIAN\n");
    return 0;
}
