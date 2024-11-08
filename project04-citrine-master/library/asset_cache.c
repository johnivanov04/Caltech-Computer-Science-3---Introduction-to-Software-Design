#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL.h>
#include <assert.h>

#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

//takes the already made renderer type from sdl_wrapper where it is already defined
extern SDL_Renderer *renderer;

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  if (entry == NULL){
    return;
  }

  switch (entry->type){
    case ASSET_IMAGE: {
      SDL_DestroyTexture((SDL_Texture*) entry->obj);
      break;
    }
    case ASSET_FONT: {
      TTF_CloseFont((TTF_Font*) entry->obj);
      break;
    }
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

void *already_exists(asset_type_t ty, const char *filepath){
  for (size_t i = 0; i < list_size(ASSET_CACHE); i++){
    entry_t *entry = (entry_t *)list_get(ASSET_CACHE, i);
    assert(entry != NULL);
    if (entry && strcmp(entry->filepath, filepath) == 0){
      assert(entry->type == ty);
      return entry->obj;
    }
  }
  return NULL;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  void *exist_object = already_exists(ty, filepath);
  if (exist_object != NULL){
    return exist_object;
  }

  entry_t *new_entry = malloc(sizeof(entry_t));
  assert(new_entry != NULL);
  new_entry->type = ty;
  new_entry->filepath = filepath;
  

  switch (ty) {
    case ASSET_IMAGE: {
      new_entry->obj = IMG_LoadTexture(renderer, filepath);
      break;
    }
    case ASSET_FONT: {
      new_entry->obj = TTF_OpenFont(filepath, FONT_SIZE);
      break;
    }
    default: {
      return NULL;
    }
  }
  list_add(ASSET_CACHE, new_entry);
  return new_entry->obj;
}
