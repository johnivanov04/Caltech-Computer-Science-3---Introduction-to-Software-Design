#include "sdl_wrapper.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct state {
  SDL_Texture *texture1;
  SDL_Texture *texture2;
  SDL_Texture *texture3;
  SDL_Texture *curr_texture;
  double clock_time;
} state_t;

state_t *emscripten_init() { 
  state_t *state = malloc(sizeof(state_t));
  vector_t min = {0.0, 0.0};
  vector_t max = {WIDTH, HEIGHT};
  sdl_init(min, max);
  state->texture1 = sdl_display("assets/cs2_logo.png");
  state->texture2 = sdl_display("assets/cs3_logo.png");
  state->texture3 = text_display("Clock: 0");
  state->curr_texture = state->texture1;
  state->clock_time = 0.0;

  return state;
}

void emscripten_main(state_t *state){
  sdl_clear();
  // update the clock for every second that passes
  double delta_t = time_since_last_tick();
  while (delta_t < 1.0){
    delta_t += time_since_last_tick();
  }
  state->clock_time += delta_t;
  char* text = malloc((sizeof(char) * 7) + sizeof(int));
  sprintf (text, "Clock:%d", (int)state->clock_time);
  state->texture3 = text_display(text);
  text_render(state->texture3);
  
  // update logo every 5 seconds
  if ((int) state->clock_time % 10 > 4){
    state->curr_texture = state->texture2;
  }else{
    state->curr_texture = state->texture1;
  }
 
  sdl_render(state->curr_texture);

  sdl_show();
  free(text);   
}

void emscripten_free(state_t *state) {
  SDL_DestroyTexture(state->texture1);
  SDL_DestroyTexture(state->texture2);
  SDL_DestroyTexture(state->curr_texture);
  SDL_DestroyTexture(state->texture3);
  free(state);
}