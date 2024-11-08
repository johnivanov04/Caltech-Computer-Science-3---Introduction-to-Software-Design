#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const rgb_color_t BLUE = (rgb_color_t){0, 0, 255};

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const size_t NUM_MEMES = 3;

const size_t TEXT_SIZE = 18;
const size_t TEXT_HEIGHT_SCALE = 2;

struct state {
  list_t *memes;
  double time;
  size_t meme;
};

typedef struct meme {
  size_t num_captions;
  char **captions;
  vector_t *locations;
  const char *font_path;
  const char *bg_path;
  vector_t bg_loc;
  vector_t bg_size;
} meme_t;

meme_t meme_templates[] = {
    {.num_captions = 3,
     .captions = (char *[]){"MEMORY LEAKS", "ASAN", "CS3 STUDENTS"},
     .locations = (vector_t[]){(vector_t){270, 360}, (vector_t){545, 250},
                               (vector_t){585, 330}},
     .font_path = "assets/impact.ttf",
     .bg_path = "assets/asan_meme.png",
     .bg_loc = (vector_t){170, 0},
     .bg_size = (vector_t){660, 440}},
    {
        .font_path = "assets/Cascadia.ttf",
        .bg_path = "assets/code_quality_meme.png",
        .num_captions = 6,
        .bg_loc = (vector_t){250, 0},
        .bg_size = (vector_t){500, 500},
        .captions =
            (char *[]){"if flag == true {", "    return true;", "} else {",
                       "    return false;", "}", "return flag;"},
        .locations = (vector_t[]){(vector_t){530, 30}, (vector_t){530, 70},
                                  (vector_t){530, 110}, (vector_t){530, 150},
                                  (vector_t){530, 190}, (vector_t){530, 350}},
    },
    {
        .font_path = "assets/impact.ttf",
        .bg_path = "assets/malloc_meme.png",
        .num_captions = 2,
        .bg_loc = (vector_t){250, 0},
        .bg_size = (vector_t){500, 500},
        .captions =
            (char *[]){"FOR ALL MALLOC I SEE", "A FREE THERE SHOULD BE"},
        .locations = (vector_t[]){(vector_t){300, 50}, (vector_t){300, 450}},
    }};


/**
 * Returns a 2D array of assets that are grouped by the meme they belong to. In other
 * words, the `i`th index of the list should be a list of assets corresponding
 * to the `i`th meme.
 *
 * @param state the state of the game
 * @return a 2-D list of assets to be rendered
 */
list_t *generate_memes(state_t *state) {
  list_t *memes = list_init(NUM_MEMES, (free_func_t)list_free);

  for (size_t i = 0; i < NUM_MEMES; i++){
    meme_t template = meme_templates[i];
    list_t *meme_init = list_init(template.num_captions+1, free);

    SDL_Rect bg_rect;
    bg_rect.x = (int)template.bg_loc.x;
    bg_rect.y = (int)template.bg_loc.y;
    bg_rect.w = (int)template.bg_size.x;
    bg_rect.h = (int)template.bg_size.y;
    
    asset_t *bg_asset = asset_make_image(template.bg_path, bg_rect);
    list_add(meme_init, bg_asset);

    for (size_t j = 0; j < template.num_captions; j++){
      SDL_Rect caption_rect = {template.locations[j].x, template.locations[j].y, template.bg_size.x, TEXT_SIZE*TEXT_HEIGHT_SCALE};
      asset_t *caption_asset = asset_make_text(template.font_path, caption_rect, template.captions[j], BLUE);
      list_add(meme_init, caption_asset);
    }
    list_add(memes, meme_init);
  }
  return memes;
}

/**
 * Cycles the meme variable of state_t *state. Should be the "callback" function
 * for the mouse variable.
 *
 * @param state the state of the game
 * @param x x position of the click
 * @param y y position of the click
 */
void cycle_meme(state_t *state, double x, double y) {
  state->meme += 1;
  state->meme = state->meme % list_size(state->memes);
  fprintf(stderr, "%zu\n", state->meme);
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  assert(state);

  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  
  state->time = 0;
  state->memes = generate_memes(state);
  state->meme = 0;

  sdl_on_click((void *)cycle_meme);
  return state;
}

void emscripten_main(state_t *state) {
  sdl_clear();
  state->time += time_since_last_tick();

  list_t *memes = list_get(state->memes, state->meme);
  for (size_t i = 0; i < list_size(memes); i++) {
    asset_render(list_get(memes, i));
  }
  sdl_show();
}

void emscripten_free(state_t *state) {
  TTF_Quit();
  list_free(state->memes);
  free(state);
}
