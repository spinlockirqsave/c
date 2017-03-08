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
    fprintf(stderr, "%s <file> <character to be searched and counted>\n\n", name);
}

int main(int argc, char *argv[])
{
    FILE    *fp;
    char    buf[BUF_SZ]= {0};
    int     val = 0;
	size_t	read = 0, read_now = 0, count = 0;

    if (argc < 2) {
        fprintf(stderr, "Program takes file name and character as argument.\n\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        fprintf(stderr, "Error opening file.\nPlease check the file name.\n\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }
	val = *argv[2];

    while ((read_now = fread(buf, 1, BUF_SZ, fp)) > 0) {
		read += read_now;
		int i = 0;
		for (; i < read_now; ++i) {
			if (buf[i] == val)
				++count;
		}
    }
	fprintf(stderr, "Bytes read: [%zu]\n\n", read);
	fprintf(stderr, "Count: [%zu]\n\n", count);

    return EXIT_SUCCESS;
}
