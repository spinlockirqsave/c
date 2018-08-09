/*
 * @file	main.c
 * @author	Piotr Gregor <piotr dataandsignal.com>
 * @brief	Replace string pattern in file with bytes.
 * 		Return number of substitutions made.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>


void
usage(const char *name)
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr, "%s <in file> <string pattern> <replace bytes> <out file>\n\n", name);
    fprintf(stderr, "<in file>\t\tinput file\n");
    fprintf(stderr, "<string pattern>\ttext to be substituted, escape slash with slash, e.g. type \"\\r\\n\" to replace 4 chars \"slash r slash n\" in text\n");
    fprintf(stderr, "<replace bytes>\t\tbytes in hex (2 chars per hex number) which replace searched pattern, type 0d0a to replace with carriage return and line break\n");
    fprintf(stderr, "<output file>\t\tname of the result (created or trunced)\n");
}

char *str_replace(char *orig, const char *rep, uint8_t *with, size_t wlen, size_t *cnt)
{
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (cnt) *cnt = 0;

    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    if (cnt) *cnt = count;
    tmp = result = malloc(strlen(orig) + (wlen - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = (char*) memcpy(tmp, with, wlen) + wlen;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

int main(int argc, char *argv[])
{
    FILE    	*fp = NULL, *fpout = NULL;
    char    	*buf = NULL, *res = NULL;
    const char	*pattern = NULL, *replace_pattern = NULL;
    uint8_t	*rp = NULL;
    size_t	read = 0, fplen = 0, count = 0, plen = 0, rplen = 0;

    struct stat st = {};


    if (argc < 5 || strlen(argv[3]) < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1], "r");
    
    if (fp == NULL) {
	    
	    fprintf(stderr, "Error opening file.\nPlease check the file name.\n\n");
	    usage(argv[0]);
	    return EXIT_FAILURE;
    }
    
    fpout = fopen(argv[4], "w"); 
    if (fpout == NULL) {
	    
	    fprintf(stderr, "Error creating file.\n\n");
	    usage(argv[0]);
	    return EXIT_FAILURE;
    }
    
    //pattern = str_replace(argv[2], "\\\\", 0x5c);
    pattern = argv[2]; 
    plen = strlen(pattern);

    replace_pattern = argv[3];
    rplen = strlen(replace_pattern);
    rp = malloc(rplen / 2);
    if (rp == NULL) {
	    
	    fprintf(stderr, "Cannot allocate buffer (for replace pattern) of size %zu\n", plen);
	    return EXIT_FAILURE;
    }

    int i = 0;
    while (i < rplen / 2) {
	    
	    sscanf(replace_pattern, "%2hhx", &rp[i]);
	    replace_pattern += 2;

	    ++i;
    }

    stat(argv[1], &st);
    fplen = st.st_size;
    
    buf = malloc(fplen);
    if (buf == NULL) {
	    
	    fprintf(stderr, "Cannot allocate buffer (for file reading) of size %zu\n", fplen);
	    return EXIT_FAILURE;
    }
    
    if (fread(buf, fplen, 1, fp) != 1) {

	    fprintf(stderr, "Cannot read the file\n");
	    return EXIT_FAILURE;
    }
    
    res = str_replace(buf, pattern, rp, rplen / 2, &count);
    if (res == NULL) {

	    fprintf(stderr, "Error replacing the pattern\n");
	    return EXIT_FAILURE;
    }
    
    fwrite(res, 1, strlen(res), fpout);
    
    fprintf(stderr, "Done. Made %zu substitutions\n\n", count);
    
    free(buf); buf = NULL;
    free(rp); rp = NULL;
    
    return EXIT_SUCCESS;
}
