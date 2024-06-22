#include "graphics.h"
#include "disassembler.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	char* endptr;
	int video_scale = strtol(argv[2], &endptr, 10);
	float frequency = strtol(argv[3], &endptr, 10);
	load_rom(argv[1]);    

	init(VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale, frequency);
	loop();
	quit();

	return 0;
}
