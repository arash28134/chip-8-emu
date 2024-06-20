#include "graphics.h"
#include "sound.h"

#include <SDL2/SDL.h>

SDL_Renderer* rend;
SDL_Window* win;

int **pixels;

void init() {
	// returns zero on success else non-zero
    	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        	printf("error initializing SDL: %s\n", SDL_GetError());
	}

	win = SDL_CreateWindow("Chip8 Emulator", // creates a win
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	rend = SDL_CreateRenderer(win, -1, render_flags);
	
	if (!rend) {
        	SDL_Log("Could not create rend: %s", SDL_GetError());
        	SDL_DestroyWindow(win);
        	SDL_Quit();
    	}	

	pixels = (int**)malloc(SCREEN_WIDTH * sizeof(int*));

	for (int i = 0; i < SCREEN_WIDTH; i++)
		pixels[i] = calloc(SCREEN_HEIGHT, sizeof(int));

	//audio_init();
}

void draw(int x, int y, int val) {
	pixels[x][y] = val;
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

		// render each element of **pixels
		for (int x = 0; x < SCREEN_WIDTH; x++)
			for (int y = 0; y < SCREEN_HEIGHT; y++) {
				int val = pixels[x][y] * 255;

				SDL_SetRenderDrawColor(rend, val, val, val, 255);
				SDL_RenderDrawPoint(rend, x, y);
			}

		SDL_RenderPresent(rend);
        }
}

void quit() {
	SDL_DestroyRenderer(rend);
    	SDL_DestroyWindow(win);
	//audio_quit();
    	SDL_Quit();

	free(pixels);
}
