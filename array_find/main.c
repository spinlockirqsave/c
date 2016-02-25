#include <stdio.h>
#include <stdint.h>

int get_idx(int a[], uint32_t dims[], uint32_t dims_n, int value)
{
    size_t size;
    size_t index = 0, n;

    if (dims[0] < 1) return -1;

    size = 1;
    n = dims_n;
    while (n)
        size *= dims[--n];
    if (dims_n > size) return -1;
 
    while (index < size && a[index] != value)
    {
        ++index;
    }

    if (index == size)
       return  -1;
    n = 0;
    while (n < dims_n)
    {
        size /= dims[n];
        dims[n] = index / size;
        index = index % size;
        ++n;
    }
    return 0;
}

int
main()
{
    int res;
    uint32_t dims[5];
    int a[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    
    dims[0] = 12;
    res = get_idx(a, dims, 1, 10);
    if (res == -1) goto fail;
    printf("dim1 [%u]\n", dims[0]);

    dims[0] = 2;
    dims[1] = 6;
    res = get_idx(a, dims, 2, 10);
    if (res == -1) goto fail;
    printf("dim1 [%u] dim2 [%u]\n", dims[0], dims[1]);
    
    dims[0] = 3;
    dims[1] = 2;
    dims[2] = 2;
    res = get_idx(a, dims, 3, 10);
    if (res == -1) goto fail;
    printf("dim1 [%u] dim2 [%u] dim3 [%u]\n", dims[0], dims[1], dims[2]);
    
    dims[0] = 2;
    dims[1] = 2;
    dims[2] = 2;
    dims[3] = 2;
    res = get_idx(a, dims, 4, 10);
    if (res == -1) goto fail;
    printf("dim1 [%u] dim2 [%u] dim3 [%u] dim4 [%u]\n", dims[0], dims[1], dims[2], dims[3]);
    return 0;

fail:
        printf("Not found\n");
        return -1;
}
