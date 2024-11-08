#ifndef __ASSET_CACHE_H__
#define __ASSET_CACHE_H__

#include "asset.h"
#include <stddef.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL.h>

/**
 * Initializes the empty, list-based global asset cache. The caller must then
 * destroy the cache with `asset_cache_destroy` when done.
 */
void asset_cache_init();

/**
 * Frees the global asset cache and its owned contents.
 */
void asset_cache_destroy();

/**
 * Gets the pointer to the object that is associated with the given filepath.
 * If the object exists, asserts that its type matches the given type. 
 * 
 * If the object doesn't exist, adds a new entry to the asset cache and returns 
 * the pointer to the newly created object.
 * 
 * Example:
 * ```
 * char *img_path = "assets/image.png";
 * SDL_Texture *obj = asset_cache_obj_get_or_create(ASSET_IMAGE, img_path);
 * 
 * char *font_path = "assets/font.ttf";
 * TTF_Font *obj = asset_cache_obj_get_or_create(ASSET_FONT, font_path);
 * ```
 *
 * @param ty the type of the asset
 * @param filepath the filepath to the asset
 * @return the object that corresponds to the filepath, as a void*
 */
void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath);



/**
 * Checks to see if an entry already corresponds to 'filepath'. If so
 * then return the entrys object, otherwise NULL.
 */
void *already_exists(asset_type_t ty, const char *filepath);

#endif // #ifndef __ASSET_CACHE_H__
