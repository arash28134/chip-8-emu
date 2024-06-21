#include "graphics.h"
#include "disassembler.h"

#include <SDL2/SDL.h>

SDL_Renderer* rend;
SDL_Window* win;
SDL_Texture* texture;

int vid_delay;

void init(int windowWidth, int windowHeight, int delay) {
	// returns zero on success else non-zero
    	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        	printf("error initializing SDL: %s\n", SDL_GetError());
	}

	win = SDL_CreateWindow("Chip8 Emulator", // creates a win
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       windowWidth, windowHeight, 0);
	
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	rend = SDL_CreateRenderer(win, -1, render_flags);

	if (!rend) {
        	SDL_Log("Could not create rend: %s", SDL_GetError());
        	SDL_DestroyWindow(win);
        	SDL_Quit();
    	}	

	texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGB888,
                                             SDL_TEXTUREACCESS_STREAMING,
			     VIDEO_WIDTH, VIDEO_HEIGHT);
	if (!texture) {
		fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
	}

	vid_delay = delay;
}

void loop() {
	int running = 1;
	SDL_Event event;

	while (running) {
		while (SDL_PollEvent(&event)) {
            		switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {

					} break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {

					} break;
            		}
		}

		cycle();
		update_buffer();

		SDL_RenderClear(rend);
        	SDL_RenderCopy(rend, texture, NULL, NULL);	
		SDL_RenderPresent(rend);
		SDL_Delay(vid_delay);
        }
}

void update_buffer() {
	uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];
	for (int y = 0; y < VIDEO_HEIGHT; ++y) {
		for (int x = 0; x < VIDEO_WIDTH; ++x) {
			uint8_t pixel = videobuf[x + y * VIDEO_WIDTH];
			uint32_t color = pixel ? 0xFFFFFFFF : 0x00000000; // White for on, black for off
			pixels[x + y * VIDEO_WIDTH] = color;
		}
	}

	SDL_UpdateTexture(texture, NULL, pixels, VIDEO_WIDTH * sizeof(uint32_t));
}

void quit() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	//audio_quit();
	SDL_Quit();
}
