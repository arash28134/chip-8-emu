#include "disassembler.h"
#include "audio.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

uint16_t pc; // program counter
uint16_t I; // index register
uint8_t regs[16]; // 16 8-bit general-purpose regs; V0, V1,..., VF
uint8_t memory[4096]; // 4kb memory
uint16_t stack[16]; // 16 levels stack
uint8_t sp; // stack pointer
uint32_t videobuf[64 * 32]; // 64 pixels wide and 32 pixels high (32-bit to make using with SDL easier)
uint8_t keypad[16]; // 16 keys, 0 through F
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
	
	printf("0x%X PC: %X, I: %X\n", opcode, pc, I);
	for (int i = 0; i <= 16; i++)
		printf("V%d: %X ", i, regs[i]);
	printf("\n");
	
	pc += 2; // next instruction
	
	// decode and execute current opcode
	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x000F) {
				case 0x0000: // 00E0: Clear the screen
					memset(videobuf, 0, sizeof(videobuf));
					break;
				case 0x000E: // 00EE: Return from a subroutine
					pc = stack[sp];
					sp--;
					break;
				default: // 0NNN: Not necessary for most ROMs
					printf("Skipping 0x%X...HEY\n", opcode);
					break;

			}
			break;
		case 0x1000: // 1NNN: Jump to address NNN
		{
			uint16_t NNN = opcode & 0x0FFF;
			pc = NNN;
			break;
		}
		case 0x2000: // 2NNN: Call subroutine at NNN
		{
			sp += 1;
			stack[sp] = pc;
			uint16_t NNN = opcode & 0x0FFF;
			pc = NNN;
			break;
		}
		case 0x3000: // 3XNN
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			if (regs[Vx] == NN)
				pc += 2;
			break;
		}
		case 0x4000: // 4XNN
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			if (regs[Vx] != NN)
				pc += 2;
			break;
		}
		case 0x5000: // 5XY0
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t Vy = (opcode & 0x00F0) >> 4;
			if (regs[Vx] == regs[Vy])
				pc += 2;
			break;
		}
		case 0x6000: // 6XNN: Set VX to NN
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			regs[Vx] = NN;
			break;
		}
		case 0x7000: // 7XNN: Add NN to VX
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			regs[Vx] += NN;
			break;
		}
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0000:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] = regs[Vy];
					break;
				}
				case 0x0001:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] |= regs[Vy];
					break;
				}
				case 0x0002:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] &= regs[Vy];
					break;
				}
				case 0x0003:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					regs[Vx] ^= regs[Vy];
					break;
				}
				case 0x0004:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;

					uint16_t sum = regs[Vx] + regs[Vy];
					if (sum > 255)
						regs[0xF] = 1;
					else
						regs[0xF] = 0;
					regs[Vx] += regs[Vy];
					break;
				}
				case 0x0005:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;

					if (regs[Vx] > regs[Vy])
						regs[0xF] = 1;
					else
						regs[0xF] = 0;
					regs[Vx] -= regs[Vy];
					break;
				}
				case 0x0006: 
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					
					regs[0xF] = (regs[Vx] & 0x1);
					regs[Vx] >>= 1;
					break;
				}
				case 0x0007: 
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t Vy = (opcode & 0x00F0) >> 4;
					
					if (regs[Vy] > regs[Vx])
						regs[0xF] = 1;
					else
						regs[0xF] = 0;

					regs[Vx] = (regs[Vy] - regs[Vx]);	
					break;
				}
				case 0x000E:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					
					regs[0xF] = (regs[Vx] & 0x80) >> 7;
					regs[Vx] <<= 1;
					break;
				}
			}
			break;
		case 0x9000: // 9XY0
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t Vy = (opcode & 0x00F0) >> 4;
			if (regs[Vx] != regs[Vy])
				pc += 2;
			break;
		}
		case 0xA000: // ANNN: Set I to the address NNN
		{
			uint16_t NNN = opcode & 0x0FFF;
			I = NNN;
			printf("I: %X\n", I);
			break;
		}
		case 0xB000: // BNNN: Jump to the address NNN plus V0
		{
			uint16_t NNN = opcode & 0x0FFF;
			pc = NNN + regs[0];
			break;
		}
		case 0xC000: // CXNN
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t NN = opcode & 0x00FF;
			regs[Vx] = randbyte() & NN;
			break;
		}
		case 0xD000: // DXYN: Draw sprite at VX, VY
		{
			uint8_t Vx = (opcode & 0x0F00) >> 8;
			uint8_t Vy = (opcode & 0x00F0) >> 4;
			uint8_t height = opcode & 0x000F;

			// Wrap if going beyond screen boundaries
			uint8_t xPos = regs[Vx] % VIDEO_WIDTH;
			uint8_t yPos = regs[Vy] % VIDEO_HEIGHT;

			regs[0xF] = 0;

			for (unsigned int row = 0; row < height; ++row)
			{
				uint8_t spriteByte = memory[I + row];

				for (unsigned int col = 0; col < 8; ++col)
				{
					uint8_t spritePixel = spriteByte & (0x80 >> col);
					uint32_t* screenPixel = &videobuf[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

					// Sprite pixel is on
					if (spritePixel)
					{
						// Screen pixel also on - collision
						if (*screenPixel == 0xFFFFFFFF)
						{
							regs[0xF] = 1;
						}

						// Effectively XOR with the sprite pixel
						*screenPixel ^= 0xFFFFFFFF;
					}
				}
			}
			break;
		}
		case 0xE000: 
			switch (opcode & 0x000F) {
				case 0x000E:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					
					if (keypad[regs[Vx]])
						pc += 2;
					break;
				}
				case 0x0001:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					
					if (!keypad[regs[Vx]])
						pc += 2;
					break;
				}
				default:
					printf("Unknown opcode: %X\n", opcode);
					break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x000F) {
				case 0x0007:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					regs[Vx] = delay_timer;
					break;
				}
				case 0x000A: 
					uint8_t Vx = (opcode & 0x0F00) >> 8;

					if (keypad[0])
					{
						regs[Vx] = 0;
					}
					else if (keypad[1])
					{
						regs[Vx] = 1;
					}
					else if (keypad[2])
					{
						regs[Vx] = 2;
					}
					else if (keypad[3])
					{
						regs[Vx] = 3;
					}
					else if (keypad[4])
					{
						regs[Vx] = 4;
					}
					else if (keypad[5])
					{
						regs[Vx] = 5;
					}
					else if (keypad[6])
					{
						regs[Vx] = 6;
					}
					else if (keypad[7])
					{
						regs[Vx] = 7;
					}
					else if (keypad[8])
					{
						regs[Vx] = 8;
					}
					else if (keypad[9])
					{
						regs[Vx] = 9;
					}
					else if (keypad[10])
					{
						regs[Vx] = 10;
					}
					else if (keypad[11])
					{
						regs[Vx] = 11;
					}
					else if (keypad[12])
					{
						regs[Vx] = 12;
					}
					else if (keypad[13])
					{
						regs[Vx] = 13;
					}
					else if (keypad[14])
					{
						regs[Vx] = 14;
					}
					else if (keypad[15])
					{
						regs[Vx] = 15;
					}
					else
					{
						pc -= 2;
					}
				break;
				case 0x0005:
					switch (opcode & 0x00F0) {
						case 0x0010:
						{
							uint8_t Vx = (opcode & 0x0F00) >> 8;
							delay_timer = regs[Vx];
							break;
						}
						case 0x0050:
						{
							uint8_t Vx = (opcode & 0x0F00) >> 8;

							for (uint8_t i = 0; i <= Vx; ++i)
								memory[I + i] = regs[i];
							break;
						}
						case 0x0060: 
						{
							uint8_t Vx = (opcode & 0x0F00) >> 8;

							for (uint8_t i = 0; i <= Vx; ++i)
								regs[i] = memory[I + i];
							break;
						}
						default:
							printf("Unknown opcode: %X\n", opcode);
							break;
					}
				case 0x0008:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					sound_timer = regs[Vx];
					break;
				}
				case 0x000E:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					I += regs[Vx];
					break;
				}
				case 0x0009:
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					I = FONTSET_START_ADDR + (5 * regs[Vx]);
					break;
				}
				case 0x0003: 
				{
					uint8_t Vx = (opcode & 0x0F00) >> 8;
					uint8_t value = regs[Vx];

					// Ones-place
					memory[I + 2] = value % 10;
					value /= 10;

					// Tens-place
					memory[I + 1] = value % 10;
					value /= 10;

					// Hundreds-place
					memory[I] = value % 10;
					break;
				}
				default:
					printf("Unknown opcode: %X\n", opcode);
					break;
			}
		break;
		default:
			printf("Unkown opcode: 0x%X\n", opcode);
		break;
	}

	// decrement sound and delay timers if they have been set
	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer > 0) {
		beep(1);
		--sound_timer;
	}
	else
		beep(0);
}

uint8_t randbyte() {
	return (uint8_t)(rand() % 256);
}
