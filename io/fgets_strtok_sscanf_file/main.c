/*
 * @file    main.c
 * @author  Piotr Gregor <piotrek.gregor gmail com>
 * @brief   
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUF_SZ 400

void
usage(const char *name)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <file>\n\n", name);
}

int main(int argc, char *argv[])
{
    FILE    *fp;
    char    buf[BUF_SZ]= {0};
    char    delimiters[] = "//\r\n\t ";
    char    *val_tok;
    int     value, read_n;

    if (argc < 2)
    {
        fprintf(stderr, "Program takes file "
                "name as argument.\n\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file.\n"
                "Please check file name.\n\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    while(fgets(buf, BUF_SZ, fp) != NULL)
    {
        val_tok = strtok(buf, delimiters);
        read_n = sscanf(val_tok, "%d", &value);
        /* this has failed cause first byte in line is 'f'
         * character, so artificially force to continue */
        if (read_n == 0)
            fprintf(stderr, "Read failed but in this example"
                    " we know it is f letter, so let's continue\n");
        do
        {
            val_tok = strtok(NULL, delimiters);
            read_n = (val_tok == NULL ? 0 : sscanf(val_tok, "%d", &value));
            if (read_n > 0)
            {
                printf("[%d]\n", value);
            } else {
                printf("SKIPPED!\n");
            }
        } while (read_n == 1);
    }
    return EXIT_SUCCESS;
}
