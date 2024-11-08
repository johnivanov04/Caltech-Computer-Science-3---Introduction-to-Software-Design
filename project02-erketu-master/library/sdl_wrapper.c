#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"

const char WINDOW_TITLE[] = "CS 3";
const size_t WINDOW_WIDTH = 1000;
const size_t WINDOW_HEIGHT = 500;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;


SDL_Texture *sdl_display(char *path){
    SDL_Texture *img = IMG_LoadTexture(renderer, path);
    return img;
}

void sdl_render(SDL_Texture *texture){
    SDL_Rect *textr = malloc(sizeof(SDL_Rect));
    textr->x = 150;
    textr->y = 75;
    textr->w = 700;
    textr->h = 350;
    SDL_RenderCopy(renderer, texture, NULL, textr);
}

SDL_Texture *text_display(char* text){
    TTF_Init();
    TTF_Font *Roboto_Regular = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
    SDL_Color Black = {0, 0, 0};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Roboto_Regular, text, Black); 
    SDL_Texture *Text = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    return Text;

}

void text_render(SDL_Texture *Text){
    SDL_Rect *Message_rect = malloc(sizeof(SDL_Rect));
    Message_rect->x =450;
    Message_rect->y = 0;
    Message_rect->w = 150;
    Message_rect->h = 75;
    SDL_RenderCopy(renderer, Text, NULL, Message_rect);
}

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)
    };
    return pixel;
}

void sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max)),
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
}

bool sdl_is_done(void) {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    bool quit = false;
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            quit = true;
            break;
        }
    }
    free(event);
    return quit;
}

void sdl_clear(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, float r, float g, float b) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= r && r <= 1);
    assert(0 <= g && g <= 1);
    assert(0 <= b && b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        r * 255, g * 255, b * 255, 255
    );
    free(x_points);
    free(y_points);
}

void sdl_show(void) {
    SDL_RenderPresent(renderer);
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}

