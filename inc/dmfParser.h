#ifndef _DMF_PARSER_
#define _DMF_PARSER_

#define u8 unsigned char
// The system in the DMF
#define SYSTEM_GENESIS         0x02 // (SYSTEM_TOTAL_CHANNELS 10)
#define SYSTEM_GENESIS_EXT_CH3 0x12 // (SYSTEM_TOTAL_CHANNELS 13)
#define SYSTEM_SMS             0x03 // (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_GAMEBOY         0x04 // (SYSTEM_TOTAL_CHANNELS 4)
#define SYSTEM_PCENGINE        0x05 // (SYSTEM_TOTAL_CHANNELS 6)
#define SYSTEM_NES             0x06 // (SYSTEM_TOTAL_CHANNELS 5)
#define SYSTEM_C64_SID_8580    0x07 // (SYSTEM_TOTAL_CHANNELS 3)
#define SYSTEM_C64_SID_6581    0x17 // (SYSTEM_TOTAL_CHANNELS 3)
#define SYSTEM_YM2151          0x08 // (SYSTEM_TOTAL_CHANNELS 13)

typedef struct {
    u8 system;
    u8 system_total_channels;

    u8 song_name_length;
    char *song_name;
} dmf;

int parseDMF(char *filename);

#endif // _DMF_PARSER_
