#include "sdl_wrapper.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  vector_t min = {0.0, 0.0};
  vector_t max = {WIDTH, HEIGHT};
  sdl_init(min, max);
  state->texture = sdl_display("assets/cs3_logo.png");
  return state;
}

void emscripten_main(state_t *state) {
  sdl_clear();
  sdl_render(state->texture);
  sdl_show();
}

void emscripten_free(state_t *state) {
  SDL_DestroyTexture(state->texture);
  free(state);
}