#include "disassembler.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t pc; // program counter
uint16_t I; // index register
uint8_t regs[16]; // 16 8-bit general-purpose registers; V0, V1,..., VF
uint8_t memory[4096]; // 4kb memory
uint16_t stack[16]; // 16 levels stack
uint8_t sp; // stack pointer
uint8_t fontset[FONTSET_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

void load_rom(char *filename) {
	FILE *f = fopen(filename, "rb");    
	if (f == NULL)    
	{    
		printf("error: Couldn't open %s\n", filename);    
		exit(1);    
	}    

	// get the file size    
	fseek(f, 0L, SEEK_END);    
	int fsize = ftell(f);    
	fseek(f, 0L, SEEK_SET);    

	unsigned char *buffer = malloc(fsize);    
	fread(buffer, fsize, 1, f);    
	fclose(f);

	// load file buffer into memory
	for (long i = 0; i < fsize; ++i) {
		memory[START_ADDR + i] = buffer[i];
	}

	free(buffer);

	// load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
		memory[FONTSET_START_ADDR + i] = fontset[i];
	}

	srand(time(0));
}

void disassemble() {
}

unsigned char randbyte() {
	return (unsigned char)(rand() % 256);
}
