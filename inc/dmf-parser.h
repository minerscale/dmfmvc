#ifndef _DMF_PARSER_H_
#define _DMF_PARSER_H_

#include <stdlib.h>

#define MAX_DMF_SIZE           16777216

// The system in the DMF
#define SYSTEM_GENESIS         0x02 // (SYSTEM_TOTAL_CHANNELS 10)
#define SYSTEM_GENESIS_EXT_CH3 0x12 // (SYSTEM_TOTAL_CHANNELS 13)
#define SYSTEM_SMS             0x03 // (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_GAMEBOY         0x04 // (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_PCENGINE        0x05 // (SYSTEM_TOTAL_CHANNELS 6)
#define SYSTEM_NES             0x06 // (SYSTEM_TOTAL_CHANNELS 5)
#define SYSTEM_C64_SID_8580    0x07 // (SYSTEM_TOTAL_CHANNELS 3)
#define SYSTEM_C64_SID_6581    0x47 // (SYSTEM_TOTAL_CHANNELS 3) // Suposed to be 0x17 was 71
#define SYSTEM_YM2151          0x08 // (SYSTEM_TOTAL_CHANNELS 13)

#define REIGON_PAL             0x00
#define REIGON_NTSC            0x01

#define MODE_STD               0x00
#define MODE_FM                0x01

// Helper structs:
typedef struct {
    unsigned char AM;
    unsigned char AR;
    unsigned char DR;
    unsigned char MULT;
    unsigned char RR;
    unsigned char SL;
    unsigned char TL;
    unsigned char DT2;
    unsigned char RS;
    unsigned char DT;
    unsigned char D2R;
    unsigned char SSGMODE; // (BIT 4 = 0 Disabled, 1 Enabled, BITS 0,1,2 SSG_MODE)
} FM_operator;

typedef struct {
    unsigned char name_length;
    unsigned char *name;
    unsigned char mode;

    // FM Info
        unsigned char ALG;
        unsigned char FB;
        unsigned char LFO; // (FMS on YM2612, PMS on YM2151)
        unsigned char LFO2; // (AMS on YM2612, AMS on YM2151)

        // A list of bits for the FM operators, if there is no FM
        // This will be NULL. BEWARE!
        FM_operator *FM_operators;

    // STD Info
        // Volume Macro:
            // volume_envelope is void pointer
            // when gameboy, BEWARE!!!
            unsigned char volume_envelope_size; // 0 - 127
            unsigned int *volume_envelope; // WILL BE NULL IF GAMEBOY
            signed char volume_loop_position; // -1 = NO_LOOP

        // Arpeggio Macro:
            unsigned char arpeggio_envelope_size;
            signed int *arpeggio_envelope;
            signed char arpeggio_loop_position; // -1 = NO_LOOP
            unsigned char arpeggio_macro_mode; // (0 = Normal, 1 = Fixed)

        // Duty/Noise Macro:
            unsigned char duty_noise_envelope_size;
            unsigned int *duty_noise_envelope;
            signed char duty_noise_loop_position; // -1 = NO_LOOP

        // Wavetable Macro:
            unsigned char wavetable_envelope_size;
            unsigned int *wavetable_envelope;
            signed char wavetable_loop_position; // -1 = NO_LOOP

        // PER SYSTEM DATA:
            // C64:
                unsigned char c64_triangle_wave_enabled;
                unsigned char c64_saw_wave_enabled;
                unsigned char c64_pulse_wave_enabled;
                unsigned char c64_noise_wave_enabled;
                unsigned char c64_attack;
                unsigned char c64_decay;
                unsigned char c64_sustain;
                unsigned char c64_release;
                unsigned char c64_pulse_width;
                unsigned char c64_ring_modulation_enabled;
                unsigned char c64_sync_modulation_enabled;
                unsigned char c64_to_filter;
                unsigned char c64_volume_macro_to_filter_cutoff_enabled;
                unsigned char c64_use_filter_values_from_instrument;
                //filter_globals
                unsigned char c64_filter_resonance;
                unsigned char c64_filter_cutoff;
                unsigned char c64_filter_high_pass;
                unsigned char c64_filter_band_pass;
                unsigned char c64_filter_low_pass;
                unsigned char c64_filter_ch2_off;
            // GAMEBOY
                unsigned char gb_envelope_volume;
                unsigned char gb_envelope_direction;
                unsigned char gb_envelope_length;
                unsigned char gb_sound_length;
}instrument;

// Wavetable shite
typedef struct {
    unsigned int size;
    unsigned int *data;
} wavetable;

// Notes
typedef struct {
    signed short code;
    signed short value;
} note_command;

typedef struct {
    unsigned short note;
    unsigned short octave;
    signed short volume;
    note_command *commands;
    signed short instrument;
} note_row;

typedef struct {
    unsigned char effect_columns_count;
    note_row **rows;
} note_channel;

// Samples
typedef struct {
    unsigned int size; // 4 bytes much?
    unsigned char name_length;
    unsigned char *name;

    unsigned char rate;
    unsigned char pitch;
    unsigned char amp;
    unsigned char bits; // 8 or 16

    unsigned short *data;
} sample;

// Actual DMF
typedef struct {
    // SYSTEM SET
    unsigned char system;
    unsigned char system_total_channels;

    // VISUAL INFORMATION
    unsigned char name_length;
    char *name;

    unsigned char author_length;
    char *author;

    unsigned char highlight_A;
    unsigned char highlight_B;

    // MODULE INFORMATION
    unsigned char time_base;
    unsigned char tick_time_1;
    unsigned char tick_time_2;

    unsigned char reigon; // 0 = PAL, 1 = NTSC
    unsigned char custom_hz_on;
    unsigned short custom_hz;

    int total_rows_per_pattern;
    unsigned char total_rows_in_pattern_matrix;
    unsigned char **pattern_matrix_value;

    unsigned char total_instruments;
    instrument *instruments;

    unsigned char total_wavetables;
    wavetable *wavetables;

    note_channel *channels;

    unsigned char total_samples;
    sample *samples;
} dmf;

// Reading files
int openFileIntoBuffer(char *filename, unsigned char *dest, size_t *length);
int decompressDMF(unsigned char *src, size_t src_length, unsigned char *dest);
int openDMF(char *filename, unsigned char *dest);
int parseDMF(unsigned char *decompressed_dmf, dmf *dest);
int fileToDmfType(char *filename, dmf *dest);

// Writing files
int dmfToBuffer(dmf src, unsigned char *dest, size_t *size);
int compressDMF(const unsigned char *src, size_t src_length, unsigned char *dest, size_t *dest_length);
int writeDMF(char *filename, dmf src);

// Testing.
void display_dmf(dmf song);
int verify_dmf(char *filename);

#endif // _DMF_PARSER_H_
