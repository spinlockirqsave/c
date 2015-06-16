#include <stdio.h>

union foo
{
    int i;
    char c[sizeof(int)];
};

int main()
{
    union foo f;
    f.i = 1;
    if(f.c[0] == 1)
        printf("little endian\n");
    else
        printf("big endian\n");
    return 0;
}
