#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char* create_array_A(int ssize)
{
    char *arrayA = malloc(sizeof(char) * ssize);
    return arrayA;
}

void fill_array_A(char *arrayA, int ssize)
{
    int i;
    for(i=0; i<ssize; i++)
    {
        //printf("Letter %d:\n> ", (i+1));
        arrayA[i] = (char)(48 + i);
    }
}

void show_array_A(char *arrayA, int ssize)
{
    int n = sqrt(ssize);
    int i,j=0;

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            printf("valueA[%d] = %c ", i*n + j, arrayA[i*n + j]);
            if(j == n - 1)
            {
                printf("\n");
            }
        }
    }
}

char** create_array_B(char *arrayA, int ssize)
{
    int n = sqrt(ssize);
    int i,j;

    char **arrayB = malloc(sizeof(char*) * ssize);

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            arrayB[i * n + j] = &arrayA[j*n + i];
        }
    }
    return arrayB;
}

void show_array_B(char** arrayB, int n, int m)
{
    int i,j,k=0;

    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            printf("valueB[%d] = %c ", k, *arrayB[i*n + j]);
            k ++;
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    int n = 9;

    char *arrayA = create_array_A(n);
    fill_array_A(arrayA, n);
    show_array_A(arrayA, n);

    printf("\n");

    char **arrayB = create_array_B(arrayA, n);
    show_array_B(arrayB, 3, 3);

    free(arrayA);
    free(arrayB);
    return 0;
}
