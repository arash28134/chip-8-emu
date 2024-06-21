#pragma once

#include <stdint.h>

#define START_ADDR 0x200
#define FONTSET_START_ADDR 0x50
#define FONTSET_SIZE 80 // 16 sprites from 0 to F, each 5 bytes: 16 * 5
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

extern uint32_t videobuf[64 * 32]; // 64 pixels wide and 32 pixels high (32-bit to make using with SDL easier)

void load_rom(char *filename);
void cycle();

uint8_t randbyte();
