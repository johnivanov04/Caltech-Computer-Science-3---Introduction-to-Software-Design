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
const size_t NUM_BUTTONS = 4;

const double INTERVAL = 1;

struct state {
  list_t *memes;
  double time;
  size_t meme;
  list_t *manual_buttons;
  asset_t *play_button;
  asset_t *pause_button;
  list_t *button_assets;
  bool paused;
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

typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

void next_meme(state_t *state);

void previous_meme(state_t *state);

void toggle_play(state_t *state);

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

button_info_t button_templates[] = {
    {.image_path = "assets/black_circle.png",
     .font_path = "assets/Roboto-Regular.ttf",
     .image_box = (SDL_Rect){0, 400, 100, 100},
     .text_box = (SDL_Rect){25, 425, 50, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Back",
     .handler = (void *)previous_meme},
    {.image_path = "assets/black_circle.png",
     .font_path = "assets/Roboto-Regular.ttf",
     .image_box = (SDL_Rect){900, 400, 100, 100},
     .text_box = (SDL_Rect){925, 425, 50, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Next",
     .handler = (void *)next_meme},
    {.image_path = "assets/play_button.png",
     .image_box = (SDL_Rect){0, 200, 100, 100},
     .handler = (void *)toggle_play},
    {.image_path = "assets/pause_button.png",
     .image_box = (SDL_Rect){0, 200, 100, 100},
     .handler = (void *)toggle_play}};

/**
 * Returns a 2-D array of assets that are grouped by the meme they belong to. In
 * other words, the `i`th index of the list should be a list of assets
 * corresponding to the `i`th meme.
 *
 * @param state the state of the game
 * @return a 2-D list of assets to be rendered
 */
list_t *generate_memes(state_t *state) {
  list_t *memes = list_init(NUM_MEMES, (free_func_t)list_free);

  for (size_t i = 0; i < NUM_MEMES; i++) {
    meme_t template = meme_templates[i];
    list_t *meme_init = list_init(template.num_captions + 1, free);

    SDL_Rect bg_rect;
    bg_rect.x = (int)template.bg_loc.x;
    bg_rect.y = (int)template.bg_loc.y;
    bg_rect.w = (int)template.bg_size.x;
    bg_rect.h = (int)template.bg_size.y;

    asset_t *bg_asset = asset_make_image(template.bg_path, bg_rect);
    list_add(meme_init, bg_asset);

    for (size_t j = 0; j < template.num_captions; j++) {
      SDL_Rect caption_rect = {template.locations[j].x, template.locations[j].y,
                               template.bg_size.x,
                               TEXT_SIZE * TEXT_HEIGHT_SCALE};
      asset_t *caption_asset = asset_make_text(template.font_path, caption_rect,
                                               template.captions[j], BLUE);
      list_add(meme_init, caption_asset);
    }
    list_add(memes, meme_init);
  }
  return memes;
}

void next_meme(state_t *state) {
  state->meme += 1;
  state->meme = state->meme % list_size(state->memes);
  state->paused = true;
}

void previous_meme(state_t *state) {
  // We can't just decrement by 1 in case state->meme is 0. Why won't this work?
  if (state->meme == 0) {
    state->meme = list_size(state->memes) - 1;
  } else {
    state->meme -= 1;
  }
  state->paused = true;
}

/**
 * Plays/pauses the memes depending on the curret setting.
 */
void toggle_play(state_t *state) {
  state->paused = !state->paused;
  state->time = 0;
}

/**
 * Using `info`, initializes a button in the scene.
 *
 * @param info the button info struct used to initialize the button
 */
asset_t *create_button_from_info(state_t *state, button_info_t info) {
  // Initialize the button based on `info`.
  // Don't forget to register the button!
  asset_t *image_asset = NULL;
  asset_t *text_asset = NULL;
  if (info.image_path != NULL) {
    image_asset = asset_make_image(info.image_path, info.image_box);
  }
  if (info.font_path != NULL) {
    text_asset = asset_make_text(info.font_path, info.text_box, info.text,
                                 info.text_color);
  }
  asset_t *new_button =
      asset_make_button(info.image_box, image_asset, text_asset, info.handler);
  asset_cache_register_button(new_button);
  return new_button;
}

/**
 * Initializes and stores the button assets in the state.
 */
void create_buttons(state_t *state) {
  for (size_t i = 0; i < NUM_BUTTONS; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(state, info);
    if (info.font_path == NULL) {
      if (strcmp(info.image_path, "assets/play_button.png") == 0) {
        state->play_button = button;
      } else {
        state->pause_button = button;
      }
    } else {
      list_add(state->manual_buttons, button);
    }
  }
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
  state->paused = true;
  // Note that `free_func` is NULL because `asset_cache` is reponsible for
  // freeing the button assets.
  state->manual_buttons = list_init(NUM_BUTTONS, NULL);
  // We store the assets used for buttons to be freed at the end.
  state->button_assets = list_init(NUM_BUTTONS, (free_func_t)asset_destroy);
  create_buttons(state);
  return state;
}

bool emscripten_main(state_t *state) {
  sdl_clear();
  state->time += time_since_last_tick();

  // render the memes
  list_t *memes = list_get(state->memes, state->meme);
  for (size_t i = 0; i < list_size(memes); i++) {
    asset_render(list_get(memes, i));
  }

  // render the "next" and "back" buttons
  list_t *buttons = state->manual_buttons;
  for (size_t i = 0; i < list_size(buttons); i++) {
    asset_render(list_get(buttons, i));
  }

  // render the play/pause buttons
  if (state->paused) {
    asset_render(state->play_button);
  } else {
    asset_render(state->pause_button);
  }

  // rotate between the memes if playing
  if (!state->paused && state->time > INTERVAL) {
    state->time -= INTERVAL;
    state->meme = (state->meme + 1) % list_size(state->memes);
  }

  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  // TODO: check if everything is going to be freed correctly
  TTF_Quit();
  list_free(state->memes);
  list_free(state->manual_buttons);
  list_free(state->button_assets);
  asset_cache_destroy();
  free(state);
}
