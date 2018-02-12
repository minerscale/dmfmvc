#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "dmfParser.h"
#include "test.h"

void throw(int status){
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
                printf("DMF file was not made with deflemask 12. Aborting\n");
                break;
            case 4:
                printf("Somehow this dmf isn't real system. Aborting.\n");
                break;
            case 5:
                printf("Invalid volume change amount. Aborting.\n");
                break;
            case 6:
                printf("-a is required. Aborting\n");
                break;
            default:
                printf("Unknown error. Aborting.\n");
                break;
        }
        exit(status);
    }
}

int main(int argc, char * const* argv)
{
    int c;
    char* output_str = 0;
    char* amount_str = 0;
    int invert = 0;
    int amount = -1;

    while ((c = getopt (argc, argv, "no:a:")) != -1){
        switch (c)
        {
            case 'o':
                output_str = optarg;
                break;
            case 'a':
                amount_str = optarg;
                break;
            case 'n':
                invert = 1;
                break;
            case '?':
                printf ("Usage: dmfmvc [options] <infile>\n\n-o <outfile>  Specify the output file. (defualt none)\n-a <amount>   Specify the volume change. (Can only be positive) (defualt 0)\n-n            Subtract by the amount instead of add.\n\n");
                return 1;
        }
    }

    if (optind <= argc - 2 || argv[optind] == 0){
        printf ("Usage: dmfmvc [options] <infile>\n\n-o <outfile>  Specify the output file. (defualt none)\n-a <amount>   Specify the volume change. (Can only be positive) (defualt 0)\n-n            Subtract by the amount instead of add.\n");
        return 1;
    }

    if (access(argv[optind], R_OK) == -1){
        printf ("File doesn't exist! Aborting.\n");
        return 1;
    }

    dmf song;
    int status = fileToDmfType(argv[optind], &song);
    if (status) return status;

    if ((size_t)amount_str != 0 || (size_t)amount_str != -1) amount = atoi(amount_str);
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
