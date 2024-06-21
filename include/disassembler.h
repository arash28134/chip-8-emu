#pragma once

#include <stdint.h>

#define START_ADDR 0x200
#define FONTSET_START_ADDR 0x50
#define FONTSET_SIZE 80 // 16 sprites from 0 to F, each 5 bytes: 16 * 5

void load_rom(char *filename);
void cycle();

unsigned char randbyte();
