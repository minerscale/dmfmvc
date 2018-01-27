#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "dmfParser.h"

int main(int argc, char * const* argv){
    int c;

    while ((c = getopt (argc, argv, "o:")) != -1){
        switch (c)
        {
            case 'o':
                break;
            case '?':
                if (isprint (optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                printf ("Usage: dmfmvc [-o <outfile>] <infile>\n");
                return 1;
        }
    }

    if (optind == argc){
        printf ("Usage: dmfmvc [-o <outfile>] <infile>\n");
        return 1;
    }
    if (access(argv[optind], R_OK) == -1){
        printf ("File doesn't exist! Aborting.\n");
        return 1;
    }

    int status = parseDMF(argv[optind]);
    if (status != 0){
        switch (status)
        {
            case 1:
                printf("File doesn't exist! Aborting.\n");
                break;
            case 2:
                printf("DMF file is not valid. Aborting.\n");
                break;
            default:
                printf("Unknown error. Aborting.\n");
                break;
        }
        return status;
    }

    printf("Conversion successful!\n");

    return 0;
}
