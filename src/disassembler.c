#include "disassembler.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint16_t pc; // program counter
uint16_t I; // index register
uint8_t regs[16]; // 16 8-bit general-purpose registers; V0, V1,..., VF
uint8_t memory[4096]; // 4kb memory
uint16_t stack[16]; // 16 levels stack
uint32_t videobuf[64 * 32]; // 64 pixels wide and 32 pixels high (32-bit to make using with SDL easier)
uint8_t sp; // stack pointer
uint8_t delay_timer, sound_timer;
uint16_t opcode; // chip8 opcodes are each 2 bytes long
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

	pc = START_ADDR;
}

void cycle() {
	opcode = (memory[pc] << 8) | memory[pc + 1]; 

	pc += 2;

	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x000F) {
				case 0x0000: // 00E0: Clear the screen
					memset(videobuf, 0, sizeof(videobuf));
					printf("Screen cleared.\n");
					break;
				case 0x000E: // 00EE: Return from a subroutine
					--sp; 	// top of the stack has the address of one instruction past the one that called the subroutine,
						// so we need to decrement stack pointer and put that instruction address into pc 
					pc = stack[sp];
				default: // 0NNN: Not necessary for most ROMs
					printf("Skipping 0x%X...\n", opcode);

			}
			break;
		case 0x1000: // 1NNN: Jump to address NNN
			uint16_t NNN = opcode & 0x0FFF;
			pc = NNN;
			break;
		case 0x2000: // 2NNN: Call subroutine at NNN
			++sp;
			stack[sp] = pc;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000: // 3XNN
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			if (regs[Vx] == NN)
				++pc;
			break;
		case 0x4000: // 4XNN
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			if (regs[Vx] != NN)
				++pc;
			break;
		case 0x5000: // 5XY0
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t Vy = (opcode & 0x00F0) >> 4;
			if (regs[Vx] == regs[Vy])
				++pc;
			break;
		case 0x6000: // 6XNN: Set VX to NN
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			regs[Vx] = NN;
			break;
		case 0x7000: // 7XNN: Add NN to VX
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			regs[Vx] += NN;
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0000:
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] = regs[Vy];
					break;
				case 0x0001:
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] |= regs[Vy];
					break;
				case 0x0002:
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] &= regs[Vy];
					break;
				case 0x0003:
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] ^= regs[Vy];
					break;
				case 0x0004: //
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					break;
				case 0x0005: //
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					break;
				case 0x0006: //
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					break;
				case 0x0007: //
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					break;
				case 0x000E:
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					break;
			}
			break;
		case 0x9000: // 9XY0
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t Vy = (opcode & 0x00F0) >> 4;
			if (regs[Vx] != regs[Vy])
				++pc;
			break;
		case 0xA000: // ANNN: Set I to the address NNN
			uint16_t NNN = opcode & 0x0FFF;
			I = NNN;
			printf("I: %X\n", I);
			break;
		case 0xB000: // BNNN: Jump to the address NNN plus V0
			uint16_t NNN = opcode & 0x0FFF;
			pc = NNN + regs[0];
			break;
		case 0xC000: // CXNN
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			regs[Vx] = randbyte() & NN;
			break;
		case 0xD000: // DXYN: Draw sprite at VX, VY
			break;
		case 0xE000: //
			switch (opcode & 0x000F) {
				case 0x000E:

					break;
				case 0x0001:
					
					break;
				default:
					printf("Unknown opcode: %X\n", opcode);
			}
			break;
		case 0xF000:
			switch (opcode & 0x000F) {
				case 0x0007:
				case 0x000A:
				case 0x0005:
					switch () {
						
						default:
							printf("Unknown opcode: %X\n", opcode);
					}
				case 0x0008:
				case 0x000E:
				case 0x0009:
				case 0x0003:
				default:
					printf("Unknown opcode: %X\n", opcode);
			}
			break;
		default:
			printf("Unkown opcode: 0x%X\n", opcode);
			break;
	}
}

uint8_t randbyte() {
	return (uint8_t)(rand() % 256);
}
