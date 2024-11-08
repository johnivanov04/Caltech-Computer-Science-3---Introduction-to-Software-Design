#include "scene.h"
#include "sdl_wrapper.h"
#include "state.h"
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};
const char *asan_font = "assets/impact.ttf";
const char *asan_bg_path = "assets/asan_meme.png";
const int8_t num_asan_captions = 3;
const int8_t asan_font_size = 48;
const vector_t asan_caption_dimensions = (vector_t){175, 50};
const vector_t asan_bg_loc = (vector_t){170, 0};
const vector_t asan_bg_size = (vector_t){660, 440};
const char *asan_captions[] = {"MEMORY LEAKS", "ASAN", "CS3 STUDENTS"};
const vector_t asan_positions[] = {(vector_t){270, 360}, (vector_t){545, 250},
                                   (vector_t){585, 330}};

const char *code_quality_font = "assets/Cascadia.ttf";
const char *code_quality_bg_path = "assets/code_quality_meme.png";
const int8_t num_code_quality_captions = 6;
const int8_t code_quality_font_size = 24;
const vector_t code_qaulity_caption_dimensions = (vector_t){150, 20};
const vector_t code_quality_bg_loc = (vector_t){250, 0};
const vector_t code_quality_bg_size = (vector_t){500, 500};
const char *code_quality_captions[] = {"if flag == true {",
                                       "    return true;",
                                       "} else {",
                                       "    return false;",
                                       "}",
                                       "return flag;"};
const vector_t code_quality_positions[] = {
    (vector_t){530, 30},  (vector_t){530, 70},  (vector_t){530, 110},
    (vector_t){530, 150}, (vector_t){530, 190}, (vector_t){530, 350}};

const char *malloc_font = "assets/impact.ttf";
const char *malloc_bg_path = "assets/malloc_meme.png";
const int8_t num_malloc_captions = 2;
const int8_t malloc_font_size = 96;
const vector_t malloc_caption_dimensions = (vector_t){400, 40};
const vector_t malloc_bg_loc = (vector_t){250, 0};
const vector_t malloc_bg_size = (vector_t){500, 500};
const char *malloc_captions[] = {"FOR ALL MALLOC I SEE",
                                 "A FREE THERE SHOULD BE"};
const vector_t malloc_positions[] = {(vector_t){300, 50}, (vector_t){300, 450}};

const int8_t NUM_MEMES = 3;

typedef struct meme {
  SDL_Texture *bg;
  const char **captions;
  const vector_t *locations;
  int8_t num_captions;
  TTF_Font *font;
  int8_t FONT_SIZE;
  vector_t caption_dimension;
  vector_t bg_loc;
  vector_t bg_size;
} meme_t;

typedef struct state {
  meme_t *memes;
  size_t current_meme_index;
  size_t total_memes;
} state_t;

state_t *emscripten_init() {
  sdl_init(MIN, MAX);

  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  state->memes = malloc(NUM_MEMES * sizeof(meme_t));
  assert(state->memes != NULL);
  state->current_meme_index = 0;
  state->total_memes = NUM_MEMES;

  // Paths and additional properties for each meme
  const char *paths[NUM_MEMES] = {asan_bg_path, code_quality_bg_path,
                                  malloc_bg_path};
  const char **captions[NUM_MEMES] = {asan_captions, code_quality_captions,
                                      malloc_captions};
  const vector_t *positions[NUM_MEMES] = {
      asan_positions, code_quality_positions, malloc_positions};
  const size_t num_captions[NUM_MEMES] = {
      num_asan_captions, num_code_quality_captions, num_malloc_captions};
  const size_t font_sizes[NUM_MEMES] = {asan_font_size, code_quality_font_size,
                                        malloc_font_size};
  const char *fonts[NUM_MEMES] = {asan_font, code_quality_font, malloc_font};
  const vector_t bg_sizes[NUM_MEMES] = {asan_bg_size, code_quality_bg_size,
                                        malloc_bg_size};
  const vector_t caption_dimensions[NUM_MEMES] = {
      asan_caption_dimensions, code_qaulity_caption_dimensions,
      malloc_caption_dimensions};
  const vector_t bg_locs[NUM_MEMES] = {asan_bg_loc, code_quality_bg_loc,
                                       malloc_bg_loc};

  for (int i = 0; i < NUM_MEMES; i++) {
    state->memes[i].bg = sdl_display(paths[i]);
    state->memes[i].captions = captions[i];
    state->memes[i].locations = positions[i];
    state->memes[i].num_captions = num_captions[i];
    state->memes[i].font = fonts[i];
    state->memes[i].FONT_SIZE = font_sizes[i];
    state->memes[i].bg_loc = bg_locs[i];
    state->memes[i].caption_dimension = caption_dimensions[i];
    state->memes[i].bg_size = bg_sizes[i];
  }

  return state;
}

void emscripten_main(state_t *state) {
  sdl_clear();
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      state->current_meme_index =
          (state->current_meme_index + 1) % (state->total_memes);

      printf("Mouse clicked at (%d, %d)\n", event.button.x, event.button.y);
    }
  }
  meme_t *current_meme = &state->memes[state->current_meme_index];

  sdl_render(current_meme->bg, current_meme->bg_loc.x, current_meme->bg_loc.y,
             current_meme->bg_size.x, current_meme->bg_size.y);

  for (int i = 0; i < current_meme->num_captions; i++) {
    SDL_Texture *text_texture = text_display(
        current_meme->captions[i], current_meme->font, current_meme->FONT_SIZE);
    if (text_texture) {
      text_render(text_texture, current_meme->locations[i].x,
                  current_meme->locations[i].y,
                  current_meme->caption_dimension.x,
                  current_meme->caption_dimension.y);
      SDL_DestroyTexture(
          text_texture); // Clean up the texture to avoid memory leaks
    }
  }

  sdl_show();
}

void emscripten_free(state_t *state) {
  free(state->memes);
  free(state);
}
