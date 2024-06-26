#include "audio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <math.h>
#include <stdio.h>

// constants for the beep
#define SAMPLE_RATE 44100
#define AMPLITUDE 28000
#define FREQUENCY 440
#define M_PI 3.1415926535897932384626433

// audio callback function to generate the waveform
void audio_callback(void *userdata, Uint8 *stream, int len) {
  static double phase = 0.0;
  double phase_step = 2.0 * M_PI * FREQUENCY / SAMPLE_RATE;
  Sint16 *buffer = (Sint16 *)stream;
  int length = len / 2; // because we are generating 16-bit samples

  for (int i = 0; i < length; i++) {
    buffer[i] = (Sint16)(AMPLITUDE * sin(phase));
    phase += phase_step;
    if (phase >= 2.0 * M_PI) {
      phase -= 2.0 * M_PI;
    }
  }
}

void audio_init() {
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
    printf("Failed to initialize SDL: %s\n", SDL_GetError());
  }

  SDL_AudioSpec desired_spec;
  SDL_AudioSpec obtained_spec;

  desired_spec.freq = SAMPLE_RATE;
  desired_spec.format = AUDIO_S16SYS;
  desired_spec.channels = 1; // Mono sound
  desired_spec.samples = 4096;
  desired_spec.callback = audio_callback;
  desired_spec.userdata = NULL;

  if (SDL_OpenAudio(&desired_spec, &obtained_spec) < 0) {
    printf("Failed to open Audio Device: %s\n", SDL_GetError());
  }
}

void beep(int onoff) {
  if (onoff == 1) {
    SDL_PauseAudio(0);
  } else {
    SDL_PauseAudio(1);
  }
}

void audio_quit() { SDL_CloseAudio(); }
