#include "graphics.h"
#include "disassembler.h"
#include "audio.h"

#include <SDL2/SDL.h>
#include <sys/time.h>

SDL_Renderer* rend;
SDL_Window* win;
SDL_Texture* texture;

int clock_rate;
int ms_per_cycle;

void init(int windowWidth, int windowHeight, int rate) {
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

	audio_init();

	ms_per_cycle = 1000 / rate;
}

void loop() {
	int running = 1;
	SDL_Event event;

	Uint32 last_time = SDL_GetTicks();
    	Uint32 current_time, elapsed_time;
	int cycles_to_run;

	while (running) {
		while (SDL_PollEvent(&event)) {
            		switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							{
							running = 0;
						} break;

						case SDLK_x:
							{
								keypad[0] = 1;
						} break;

						case SDLK_1:
							{
								keypad[1] = 1;
						} break;

						case SDLK_2:
							{
								keypad[2] = 1;
						} break;

						case SDLK_3:
							{
								keypad[3] = 1;
						} break;

						case SDLK_q:
							{
								keypad[4] = 1;
						} break;

						case SDLK_w:
							{
								keypad[5] = 1;
						} break;

						case SDLK_e:
							{
								keypad[6] = 1;
						} break;

						case SDLK_a:
							{
								keypad[7] = 1;
						} break;

						case SDLK_s:
							{
								keypad[8] = 1;
						} break;

						case SDLK_d:
							{
								keypad[9] = 1;
						} break;

						case SDLK_z:
							{
								keypad[0xA] = 1;
						} break;

						case SDLK_c:
							{
								keypad[0xB] = 1;
						} break;

						case SDLK_4:
							{
								keypad[0xC] = 1;
						} break;

						case SDLK_r:
							{
								keypad[0xD] = 1;
						} break;

						case SDLK_f:
							{
								keypad[0xE] = 1;
						} break;

						case SDLK_v:
							{
								keypad[0xF] = 1;
						} break;
				} break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
						case SDLK_x:
							{
								keypad[0] = 0;
						} break;

						case SDLK_1:
							{
								keypad[1] = 0;
						} break;

						case SDLK_2:
							{
								keypad[2] = 0;
						} break;

						case SDLK_3:
							{
								keypad[3] = 0;
						} break;

						case SDLK_q:
							{
								keypad[4] = 0;
						} break;

						case SDLK_w:
							{
								keypad[5] = 0;
						} break;

						case SDLK_e:
							{
								keypad[6] = 0;
						} break;

						case SDLK_a:
							{
								keypad[7] = 0;
						} break;

						case SDLK_s:
							{
								keypad[8] = 0;
						} break;

						case SDLK_d:
							{
								keypad[9] = 0;
						} break;

						case SDLK_z:
							{
								keypad[0xA] = 0;
						} break;

						case SDLK_c:
							{
								keypad[0xB] = 0;
						} break;

						case SDLK_4:
							{
								keypad[0xC] = 0;
						} break;

						case SDLK_r:
							{
								keypad[0xD] = 0;
						} break;

						case SDLK_f:
							{
								keypad[0xE] = 0;
						} break;

						case SDLK_v:
							{
								keypad[0xF] = 0;
						} break;

				} break;
			}
		}

		current_time = SDL_GetTicks();
        	elapsed_time = current_time - last_time;

		if (elapsed_time >= ms_per_cycle) {
			cycles_to_run = elapsed_time / ms_per_cycle;
			for (int i = 0; i < cycles_to_run; i++)
				cycle();
			last_time = current_time;



			SDL_UpdateTexture(texture, NULL, videobuf, VIDEO_WIDTH * sizeof(uint32_t));

			SDL_RenderClear(rend);
			SDL_RenderCopy(rend, texture, NULL, NULL);	
			SDL_RenderPresent(rend);
		}
        }
}

void quit() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	audio_quit();
	SDL_Quit();
}
