#include "miniz.h"

int openDMF(char *filename, unsigned char *dest){
	FILE *fp = fopen(filename, "rb");

	mz_ulong dest_len = 16777216;

	fseek(fp, 0, SEEK_END);
	mz_ulong lengthOfFile = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char *buffer = malloc(lengthOfFile + 1);
	if(fread(buffer, lengthOfFile, 1, fp) == 0){
		fclose(fp);
		return 1; // Unable to open file
	}
	fclose(fp);


	int cmp_status = uncompress(dest, &dest_len, buffer, lengthOfFile);
	if (cmp_status) return 2; // Not valid DMF
	return 0; // Successful
}

int parseDMF(char *filename){
	unsigned char *dmf = malloc(16777216);
	int status = openDMF(filename, dmf);
	if(status){
		return status;
	}
	FILE *fp = fopen("test.bin", "wb");
	if (fwrite(dmf, 16777216, 1, fp));

	return 0; // Successful
}
