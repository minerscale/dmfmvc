#include "dmfParser.h"
#include "miniz.h"

int openDMF(char *filename, unsigned char *dest)
{
    FILE *fp = fopen(filename, "rb");

    // Max size of decompressed DMF is 16MiB
    // Shouldn't be a problem though
    mz_ulong dest_len = 16777216; 

    // Get size of file
    fseek(fp, 0, SEEK_END);
    mz_ulong lengthOfFile = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read the file into a buffer
    unsigned char *buffer = (u8 *)malloc(lengthOfFile);
    if(fread(buffer, lengthOfFile, 1, fp) == 0){
        fclose(fp);
        return 1; // Unable to open file
    }
    fclose(fp);

    // Decompress the dmf and store it in the buffer
    int cmp_status = uncompress(dest, &dest_len, buffer, lengthOfFile);
    if (cmp_status) return 2; // Not valid DMF

    free (buffer);
    return 0; // Successful
}

int parseDMF(char *filename, dmf *ret)
{
    // Decompress the dmf
    unsigned char *dmfRoot = (u8 *)malloc(16777216);
    int status = openDMF(filename, dmfRoot);
    if(status){
        return status;
    }

    unsigned char *dmfp = dmfRoot; // Pointer to the current spot

    // Get the version and quit if it's invalid
    dmfp += 16;
    if (*dmfp != 24) return 3; // Invalid version

    // Get the system
    ++dmfp;
    ret->system = *dmfp;

    // Set the total channels based off system
    switch (ret->system){
        case SYSTEM_GENESIS:
            ret->system_total_channels = 10;
            break;
        case SYSTEM_GENESIS_EXT_CH3:
            ret->system_total_channels = 13;
            break;
        case SYSTEM_SMS:
            ret->system_total_channels = 4;
            break;
        case SYSTEM_GAMEBOY:
            ret->system_total_channels = 4;
            break;
        case SYSTEM_PCENGINE:
            ret->system_total_channels = 6;
            break;
        case SYSTEM_NES:
            ret->system_total_channels = 5;
            break;
        case SYSTEM_C64_SID_8580:
            ret->system_total_channels = 3;
            break;
        case SYSTEM_C64_SID_6581:
            ret->system_total_channels = 3;
            break;
        case SYSTEM_YM2151:
            ret->system_total_channels = 13;
            break;
        default:
            return 4; // Not a real system???
    }

    // Get the song name and the length
    ++dmfp;
    ret->name = (char *)calloc(*dmfp + 1, 1);
    memcpy (ret->name, dmfp + 1, *dmfp);
    ret->name_length = *dmfp;

    dmfp += *dmfp + 1;

    // Get the song author and the length
    ret->author = (char *)calloc(*dmfp + 1, 1);
    memcpy (ret->author, dmfp + 1, *dmfp);
    ret->author_length = *dmfp;

    dmfp += *dmfp + 1;

    // Get the highlights
    ret->highlight_A = *dmfp++;
    ret->highlight_B = *dmfp++;

    // Timing info
    ret->time_base = *dmfp++;
    ret->tick_time_1 = *dmfp++;
    ret->tick_time_2 = *dmfp++;

    // Frequency
    ret->reigon = *dmfp++;
    ret->custom_hz_on = *dmfp++;

    // Custom_hz in the file is stored in ASCII *why*
    ret->custom_hz = (*dmfp - 0x30 >= 0) ? (*dmfp - 0x30) * 100 : 0;
    ++dmfp;
    ret->custom_hz = (*dmfp - 0x30 >= 0) ? (ret->custom_hz + (*dmfp - 0x30) * 10) : ret->custom_hz / 10;
    ++dmfp;
    ret->custom_hz = (*dmfp - 0x30 >= 0) ? (ret->custom_hz + (*dmfp - 0x30)) : ret->custom_hz / 10;
    ++dmfp;

    // Pattern matrix stuff
    ret->total_rows_per_pattern = *dmfp;
    dmfp += 4;
    ret->total_rows_in_pattern_matrix = *dmfp++;

    // Create the instrument array, It's a dynamically allocated 2d array
    ret->pattern_matrix_value = (u8 **)malloc(ret->system_total_channels * sizeof(u8 *));

    for (int i = 0; i < ret->system_total_channels; i++){
        ret->pattern_matrix_value[i] = (u8 *)malloc(ret->total_rows_in_pattern_matrix * sizeof(u8));
    }

    // Set the dmf data
    for (int i = 0; i < ret->system_total_channels; i++){
        for (int j = 0; j < ret->total_rows_in_pattern_matrix; j++){
            ret->pattern_matrix_value[i][j] = *dmfp++;
        }
    }

    // Instrument data
    ret->total_instruments = *dmfp++;

    ret->instruments = (instrument *)malloc(sizeof(instrument) * ret->total_instruments);
    for (int i = 0; i < ret->total_instruments; ++i){
        ret->instruments[i].name_length = *dmfp;
        // Create the name array
        ret->instruments[i].name = (u8 *)calloc(*dmfp + 1, 1);
        memcpy(ret->instruments[i].name, dmfp + 1, *dmfp);
        dmfp += *dmfp + 1;

        // Get the mode of the instrument (STD/FM)
        ret->instruments[i].mode = *dmfp++;

        if (ret->instruments[i].mode == MODE_FM){
            ret->instruments[i].ALG = *dmfp++;
            ret->instruments[i].FB = *dmfp++;
            ret->instruments[i].LFO = *dmfp++;
            ret->instruments[i].LFO2 = *dmfp++;

            // Allocate the FM operators memory
            ret->instruments[i].FM_operators = (FM_operator *)malloc(4 * sizeof(FM_operator));
            for (int j = 0; j < 4; j++){
                ret->instruments[i].FM_operators[j].AM += *dmfp++;
                ret->instruments[i].FM_operators[j].AR += *dmfp++;
                ret->instruments[i].FM_operators[j].DR += *dmfp++;
                ret->instruments[i].FM_operators[j].MULT += *dmfp++;
                ret->instruments[i].FM_operators[j].RR += *dmfp++;
                ret->instruments[i].FM_operators[j].SL += *dmfp++;
                ret->instruments[i].FM_operators[j].TL += *dmfp++;
                ret->instruments[i].FM_operators[j].DT2 += *dmfp++;
                ret->instruments[i].FM_operators[j].RS += *dmfp++;
                ret->instruments[i].FM_operators[j].DT += *dmfp++;
                ret->instruments[i].FM_operators[j].D2R += *dmfp++;
                ret->instruments[i].FM_operators[j].SSGMODE += *dmfp++;
            }
        }else if (ret->instruments[i].mode == MODE_STD){
            if (ret->system != SYSTEM_GAMEBOY){
                // Volume macro
                ret->instruments[i].volume_envelope_size = *dmfp++;

                ret->instruments[i].volume_envelope = (unsigned int *)malloc(ret->instruments[i].volume_envelope_size * sizeof(int));
                for (int k = 0; k < ret->instruments[i].volume_envelope_size; k++){
                    ret->instruments[i].volume_envelope[k] = *dmfp;
                    dmfp += 4;
                }

                if (ret->instruments[i].volume_envelope_size > 0){
                    ret->instruments[i].volume_loop_position = *dmfp++;
                }
            }

            // Arpeggio macro
            ret->instruments[i].arpeggio_envelope_size = *dmfp++;

            ret->instruments[i].arpeggio_envelope = (signed int *)malloc(ret->instruments[i].arpeggio_envelope_size * sizeof(int));
            for (int k = 0; k < ret->instruments[i].arpeggio_envelope_size; k++){
                ret->instruments[i].arpeggio_envelope[k] = *dmfp;
                dmfp += 4;
            }

            if (ret->instruments[i].arpeggio_envelope_size > 0){
                ret->instruments[i].arpeggio_loop_position = *dmfp++;
            }
            ret->instruments[i].arpeggio_macro_mode = *dmfp++;

            // Duty/Noise macro:
            ret->instruments[i].duty_noise_envelope_size = *dmfp++;

            ret->instruments[i].duty_noise_envelope = (unsigned int *)malloc(ret->instruments[i].duty_noise_envelope_size * sizeof(int));
            for (int k = 0; k < ret->instruments[i].duty_noise_envelope_size; k++){
                ret->instruments[i].duty_noise_envelope[k] = *dmfp;
                dmfp += 4;
            }

            if (ret->instruments[i].duty_noise_envelope_size > 0){
                ret->instruments[i].duty_noise_loop_position = *dmfp++;
            }

            // Wavetable macro:
            ret->instruments[i].wavetable_envelope_size = *dmfp++;

            ret->instruments[i].wavetable_envelope = (unsigned int *)malloc(ret->instruments[i].wavetable_envelope_size * sizeof(int));
            for (int k = 0; k < ret->instruments[i].wavetable_envelope_size; k++){
                ret->instruments[i].wavetable_envelope[k] = *dmfp;
                dmfp += 4;
            }

            if (ret->instruments[i].wavetable_envelope_size > 0){
                ret->instruments[i].wavetable_loop_position = *dmfp++;
            }

            // PER SYSTEM DATA
            if (ret->system == SYSTEM_C64_SID_8580 || ret->system == SYSTEM_C64_SID_6581){
                ret->instruments[i].c64_triangle_wave_enabled = *dmfp++;
                ret->instruments[i].c64_saw_wave_enabled = *dmfp++;
                ret->instruments[i].c64_pulse_wave_enabled = *dmfp++;
                ret->instruments[i].c64_noise_wave_enabled = *dmfp++;
                ret->instruments[i].c64_attack = *dmfp++;
                ret->instruments[i].c64_decay = *dmfp++;
                ret->instruments[i].c64_sustain = *dmfp++;
                ret->instruments[i].c64_release = *dmfp++;
                ret->instruments[i].c64_pulse_width = *dmfp++;
                ret->instruments[i].c64_ring_modulation_enabled = *dmfp++;
                ret->instruments[i].c64_sync_modulation_enabled = *dmfp++;
                ret->instruments[i].c64_to_filter = *dmfp++;
                ret->instruments[i].c64_volume_macro_to_filter_cutoff_enabled = *dmfp++;
                ret->instruments[i].c64_use_filter_values_from_instrument = *dmfp++;
                ret->instruments[i].c64_filter_resonance = *dmfp++;
                ret->instruments[i].c64_filter_cutoff = *dmfp++;
                ret->instruments[i].c64_filter_high_pass = *dmfp++;
                ret->instruments[i].c64_filter_band_pass = *dmfp++;
                ret->instruments[i].c64_filter_low_pass = *dmfp++;
                ret->instruments[i].c64_filter_ch2_off = *dmfp++;
            }else if (ret->system == SYSTEM_GAMEBOY){
                ret->instruments[i].gb_envelope_volume = *dmfp++;
                ret->instruments[i].gb_envelope_direction = *dmfp++;
                ret->instruments[i].gb_envelope_length = *dmfp++;
                ret->instruments[i].gb_sound_length = *dmfp++;
            }
        }else{
            return 2; // Not a valid dmf.
        }
    }

    // Wavetables
    ret->total_wavetables = *dmfp++;
    ret->wavetables = (wavetable *)malloc(ret->total_wavetables * sizeof(wavetable));
    for (int i = 0; i < ret->total_wavetables; ++i){
        ret->wavetables[i].size = *dmfp;
        dmfp += 4;
        ret->wavetables[i].data = (unsigned int *)malloc(ret->wavetables[i].size * sizeof(unsigned int));
        for (int j = 0; j < ret->wavetables[i].size; ++j){
            ret->wavetables[i].data[j] = *dmfp;
            dmfp += 4;
        }
    }

    // Notes!
    ret->channels = malloc(ret->system_total_channels * sizeof(note_channel));
    for (int i = 0; i < ret->system_total_channels; ++i){
        ret->channels[i].effect_columns_count = *dmfp++;

        // Allocate 2D array
        ret->channels[i].rows = (note_row **)malloc(ret->total_rows_in_pattern_matrix * sizeof(note_row *));
        for (int z = 0; z < ret->total_rows_in_pattern_matrix; z++){
            ret->channels[i].rows[z] = (note_row *)malloc(ret->total_rows_per_pattern * sizeof(note_row));
        }

        for (int j = 0; j < ret->total_rows_in_pattern_matrix; ++j){
            for (int k = 0; k < ret->total_rows_per_pattern; ++k){
                ret->channels[i].rows[j][k].note = *dmfp;
                dmfp += 2;
                ret->channels[i].rows[j][k].octave = *dmfp;
                dmfp += 2;
                ret->channels[i].rows[j][k].volume = *dmfp;
                dmfp += 2;

                ret->channels[i].rows[j][k].commands = (note_command *)malloc(ret->channels[i].effect_columns_count * sizeof(note_command));
                for (int m = 0; m < ret->channels[i].effect_columns_count; ++m){
                    ret->channels[i].rows[j][k].commands[m].code = *dmfp;
                    dmfp += 2;
                    ret->channels[i].rows[j][k].commands[m].value = *dmfp;
                    dmfp += 2;
                }
                ret->channels[i].rows[j][k].instrument = *dmfp;
                dmfp += 2;
            }
        }
    }

    ret->total_samples = *dmfp++;
    ret->samples = (sample *)malloc(ret->total_samples * sizeof(sample));
    for (int i = 0; i < ret->total_samples; ++i){
        printf("Offset: %X\n", (int)(dmfp - dmfRoot));
        ret->samples[i].size = (unsigned int)*dmfp;
        dmfp += 4;

        ret->samples[i].name_length = *dmfp++;
        ret->samples[i].name = (u8 *)calloc(ret->samples[i].name_length + 1 * sizeof(u8 *), 1);
        memcpy (ret->samples[i].name, dmfp, ret->samples[i].name_length);
        dmfp += ret->samples[i].name_length + 1;

        ret->samples[i].rate = *dmfp++;
        ret->samples[i].pitch = *dmfp++;
        ret->samples[i].amp = *dmfp++;
        ret->samples[i].bits = *dmfp++;

        ret->samples[i].data = (u16 *)malloc(ret->samples[i].size * sizeof(u16));
        for (int j = 0; j < ret->samples[i].size; ++j){
            ret->samples[i].data[j] = *dmfp;
            dmfp += 2;
        }
    }

    return 0; // Successful
}
