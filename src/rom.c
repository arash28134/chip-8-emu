#include <stdio.h>
#include <stdlib.h>

char* read_rom(char *filename) {
	FILE *file;
	char *buffer;
	unsigned long fileLen;

	// open file
	file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s\n", filename);
		return NULL;
	}

	buffer = malloc(fileLen+0x200);
	fread(buffer+0x200, fileLen, 1, f);
	fclose(f);

	return buffer;
}
