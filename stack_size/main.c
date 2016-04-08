#include <stdio.h>

void func()
{
    static int i = 0;
    int arr[1024]= {0};
    printf("%zu KB pushed on stack!\n", ++i * sizeof(int));
    func();
}

int main()
{
    func();
    printf("sizeof(short) [%zu]\n", sizeof(short));
    return 0;
}
