#include "graphics.h"
#include "rom.h"

#include <stdio.h>

int main(int argc, char **argv){
	char *buf = read_rom(argv[1]);
	if (!buf)
		return 0;

	init();
	loop();
	quit();

	return 0;
}
