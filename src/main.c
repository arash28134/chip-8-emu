#include "graphics.h"
#include "rom.h"
#include "sound.h"

#include <stdio.h>

int main(int argc, char **argv){
	char *buf = read_rom("exm");
	if (!buf)
		return 0;

	int i = 0;
	printf("\n%d\n", i);

	printf("\n%d\n", i);

	init();
	loop();
	quit();

	return 0;
}
