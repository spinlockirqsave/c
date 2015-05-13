#include <stdio.h>

int main()
{
    int a[3] = {10,20,30};
    int *b = a;         // ok
    //int *b = (int*)a; // ok
    //int *b = &a;      // error
    printf( "%d\n",*b++);
    return 0;
}
