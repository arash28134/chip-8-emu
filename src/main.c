#include "graphics.h"
#include "disassembler.h"

#include <stdio.h>

int main(int argc, char **argv){
	load_rom(argv[1]);    

	return 0;
}