/* file_io.c  
 *
 * Reads integers in from a file and stores them into an array.
 * This code uses a file stream instead of a file descriptor.
 *
 * Takes a command-line argument for the filename to open 
 *
 * Usage:
 *    gcc -o file_io file_io.c
 *    ./file_io <input_file> <output_file>
 *
 * Example:
 *    ./file_io data.txt copy.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_SIZE  100000  /* Maximum elements in array */
#define MAX_NAME     256  /* Most filesystems only support 255-char filenames */

int main(int argc, char *argv[]) {
    int array[MAX_SIZE];
    int count, tmp, ret;
    char filein[MAX_NAME], fileout[MAX_NAME];
    FILE *infile, *outfile;

    /* Check if filename argument was given */
    if(argc < 3) {
        /* argv[0] contains name of executable from command-line */
        printf("Usage: %s <input_file> <output_file>\n", argv[0]); 
        exit(1);
    }

    count = 0;  /* Actual number of elements in array */
    strncpy(filein, argv[1], MAX_NAME);
    strncpy(fileout, argv[2], MAX_NAME);

    /* Open the file stream for read-only */
    infile = fopen(filein, "r");
    if(infile == NULL) {
        perror("fopen: ");
        exit(EXIT_FAILURE);
    }

    /* Open output file stream for writing */
    outfile = fopen(fileout, "w");
    if(outfile == NULL) {
        perror("fopen: ");
        exit(EXIT_FAILURE);
    }

    do {
        ret = fscanf(infile, "%d", &tmp);
        if(ret > 0) array[count++] = tmp; /* Got a number! */
    } while(ret != EOF && count < MAX_SIZE);

    printf("Read in %d integers from file %s.\n", count, filein);
    fclose(infile); /* close the file stream */

    /* Write the data to the outfile */
    for(tmp = 0; tmp < count; tmp++) {
        fprintf(outfile, "%d ", array[tmp]);
        if(tmp && ((tmp % 10) == 0)) fprintf(outfile, "\n");
    }
    fprintf(outfile, "\n");
    fclose(outfile); /* close the file stream */

    return 0;
}
