#include <stdio.h>

#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"
#include "dmf-parser.h"

// Display error based off what error code was sent
void throw(int status){
    // Pass through fine if status is 0
    // i.e. successful
    if (status != 0){
        switch (status)
        {
            case 1:
                printf("File doesn't exist! Aborting.\n");
                break;
            case 2:
                printf("DMF file is not valid. Aborting.\n");
                break;
            case 3:
                printf("DMF file was not made with Deflemask 12. Aborting\n");
                break;
            case 4:
                printf("Somehow this dmf isn't real system. Aborting.\n");
                break;
            case 5:
                printf("Invalid volume change amount. Aborting.\n");
                break;
            case 6:
                printf("'-a <value>' is required. Aborting\n");
                break;
            case 7:
                printf ("Usage: dmfmvc [options] <infile>\n\n-o <outfile>  Specify the output file. (defualt none)\n-a <amount>   Specify the volume change. (Can only be positive) (defualt 0)\n-n            Subtract by the amount instead of add.\n");
                break;
            default:
                printf("Unknown error. Aborting.\n");
                break;
        }
        exit(status);
    }
}

int main(int argc, char **argv)
{
    char* output_str = 0;
    char* amount_str = 0;
    int invert = 0;
    int amount = -1;

    char *infile;
    int option;
    struct optparse options;

    optparse_init(&options, argv);
    while ((option = optparse(&options, "no:a:")) != -1){
        switch (option)
        {
            case 'o':
                output_str = options.optarg;
                break;
            case 'a':
                amount_str = options.optarg;
                break;
            case 'n':
                invert = 1;
                break;
            case '?':
                throw(7);
                return 1;
        }
    }

    if ((infile = optparse_arg(&options)) == 0){
        throw(7);
    }

    FILE *fp = fopen(infile, "r");
    if (fp == NULL){
        throw(1);
    }
    fclose(fp);

    dmf song;
    int status = fileToDmfType(infile, &song);
    if (status) return status;

    if ((size_t)amount_str != 0 && (size_t)amount_str != -1) amount = atoi(amount_str);
    if (amount == 0) throw(5); // Not a valid number
    if (amount == -1) throw(6); // Required argument

    int system_max = 0;
    switch (song.system){
        case SYSTEM_YM2151:
        case SYSTEM_GENESIS_EXT_CH3:
        case SYSTEM_GENESIS:
            system_max = 0x7f;
            break;

        case SYSTEM_PCENGINE:
            system_max = 0x1f;
            break;

        case SYSTEM_GAMEBOY:
        case SYSTEM_SMS:
        case SYSTEM_NES:
        case SYSTEM_C64_SID_8580:
        case SYSTEM_C64_SID_6581:
            system_max = 0xf;
            break;
    }

    for (int i = 0; i < song.system_total_channels; ++i){
        for (int j = 0; j < song.total_rows_in_pattern_matrix; ++j){
            for (int k = 0; k < song.total_rows_per_pattern; ++k){
                if (song.channels[i].rows[j][k].volume != -1 && song.channels[i].rows[j][k].volume < system_max + 1){
                    if (invert == 0){
                        if (song.channels[i].rows[j][k].volume < ((system_max + 1) - amount))
                            song.channels[i].rows[j][k].volume += amount;
                        else
                            song.channels[i].rows[j][k].volume = system_max;
                    }else{
                        if (song.channels[i].rows[j][k].volume > amount)
                            song.channels[i].rows[j][k].volume -= amount;
                        else
                            song.channels[i].rows[j][k].volume = 0;
                    }
                }
            }
        }
    }

    if ((size_t)output_str != 0){
        status = writeDMF(output_str, song);
        throw(status);
    }

    return 0;
}
