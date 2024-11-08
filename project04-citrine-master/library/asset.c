#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

//takes the already made renderer type from sdl_wrapper where it is already defined
extern SDL_Renderer *renderer;

typedef struct asset {
  asset_type_t type;
  SDL_Rect bounding_box;
} asset_t;

typedef struct {
  asset_t base;
  TTF_Font *font;
  const char *text;
  rgb_color_t color;
} text_asset_t;

typedef struct {
  asset_t base;
  SDL_Texture *texture;
} image_asset_t;

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box) {
  // This is a fancy way of malloc'ing space for an `image_asset_t` if `ty` is
  // ASSET_IMAGE, and `text_asset_t` otherwise.
  asset_t *new =
      malloc(ty == ASSET_IMAGE ? sizeof(image_asset_t) : sizeof(text_asset_t));
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  image_asset_t *img_asset = (image_asset_t*) asset_init(ASSET_IMAGE, bounding_box);
  img_asset->texture = (SDL_Texture*)asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  assert(img_asset->texture);

  return (asset_t*) img_asset;
}

asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color) {
                          text_asset_t *text_asset = (text_asset_t*) asset_init(ASSET_FONT, bounding_box);
                          text_asset->font = (TTF_Font*)asset_cache_obj_get_or_create(ASSET_FONT, filepath);
                          assert(text_asset->font);
                          text_asset->text = strdup(text);
                          text_asset->color = color;
                          return (asset_t*) text_asset;
}

void asset_render(asset_t *asset) {
  switch(asset->type){
    case ASSET_IMAGE: {
      image_asset_t *img_asset = (image_asset_t*) asset;
      SDL_RenderCopy(renderer, img_asset->texture, NULL, &img_asset->base.bounding_box);
      break;
    }
    case ASSET_FONT: {
      text_asset_t *text_asset = (text_asset_t*) asset;
      SDL_Color sdl_color;
      sdl_color.r = (Uint8)text_asset->color.r;
      sdl_color.g = (Uint8)text_asset->color.g;
      sdl_color.b = (Uint8)text_asset->color.b;
      
      SDL_Surface *surface = TTF_RenderText_Solid(text_asset->font, text_asset->text, sdl_color);
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      vector_t position = (vector_t){.x = (double)text_asset->base.bounding_box.x, .y = (double)text_asset->base.bounding_box.y};
      text_display(texture, position);
      SDL_DestroyTexture(texture);
      SDL_FreeSurface(surface);
      break;
    }
  }
}

void asset_destroy(asset_t *asset) { free(asset); }
