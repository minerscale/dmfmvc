#include "dmfParser.h"
#include "miniz.h"

int openDMF(char *filename, unsigned char *dest){
	FILE *fp = fopen(filename, "rb");

	// Max size of decompressed DMF is 16MiB
	// Shouldn't be a problem though
	mz_ulong dest_len = 16777216; 

	// Get size of file
	fseek(fp, 0, SEEK_END);
	mz_ulong lengthOfFile = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Read the file into a buffer
	unsigned char *buffer = malloc(lengthOfFile);
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

int parseDMF(char *filename){
	// Decompress the dmf
	unsigned char *dmfRoot = malloc(16777216);
	int status = openDMF(filename, dmfRoot);
	if(status){
		return status;
	}

	unsigned char *dmfp = dmfRoot; // Pointer to the current spot
	dmf ret;

	// Get the version and quit if it's invalid
	dmfp += 16;
	if (*dmfp != 24) return 3; // Invalid version

	// Get the system
	dmfp += 1;
	ret.system = *dmfp;

	// Set the total channels based off system
	switch (ret.system){
		case SYSTEM_GENESIS:
			ret.system_total_channels = 10;
			break;
		case SYSTEM_GENESIS_EXT_CH3:
			ret.system_total_channels = 13;
			break;
		case SYSTEM_SMS:
			ret.system_total_channels = 4;
			break;
		case SYSTEM_GAMEBOY:
			ret.system_total_channels = 4;
			break;
		case SYSTEM_PCENGINE:
			ret.system_total_channels = 6;
			break;
		case SYSTEM_NES:
			ret.system_total_channels = 5;
			break;
		case SYSTEM_C64_SID_8580:
			ret.system_total_channels = 3;
			break;
		case SYSTEM_C64_SID_6581:
			ret.system_total_channels = 3;
			break;
		case SYSTEM_YM2151:
			ret.system_total_channels = 13;
			break;
		default:
			return 4; // Not a real system???
	}

	dmfp += 1;
	ret.song_name = calloc (*dmfp + 1, 1);
	memcpy (ret.song_name, dmfp + 1, *dmfp);

	return 0; // Successful
}
