#include "disassembler.h"
#include "graphics.h"

#include <stdlib.h>

int main(int argc, char **argv) {
  char *endptr;
  int video_scale = strtol(argv[2], &endptr, 10);
  int rate = strtol(argv[3], &endptr, 10);
  load_rom(argv[1]);

  init(VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale, rate);
  loop();
  quit();

  return 0;
}
