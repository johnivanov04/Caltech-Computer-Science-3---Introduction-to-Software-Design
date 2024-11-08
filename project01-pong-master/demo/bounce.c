#include "polygon.h"
#include "sdl_wrapper.h"
#include "state.h"
#include "vec_list.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define M_PI 3.14159265358979323846
double MAX_WIDTH = 1000.0;
double MAX_HEIGHT = 500.0;
double LENGTH = 60.0;
int STAR_POINTS = 5;

double DEFAULT_SPEED = 1.0;
double DEFAULT_ANGLE = M_PI / 4;
double DEFAULT_ROTATION_SPEED = M_PI / 8;

/**
 * Stores the demo state
 * Use this to store any variable needed every 'tick' of your demo
 */
typedef struct state {
  vec_list_t *polygon;
  vector_t *velocity;
  double rotation;
  bool just_moved;
} state_t;

vec_list_t *make_star(vector_t center, size_t length) {

  vec_list_t *poly = vec_list_init(STAR_POINTS * 2);

  for (size_t i = 0; i < STAR_POINTS; i++) {
    vector_t *top = malloc(sizeof(vector_t));
    assert(top != NULL);
    top->x = center.x;
    top->y = center.y + length;
    // using law of sines
    double height_2 = (length * sin(M_PI / (STAR_POINTS * 2))) /
                      sin(M_PI - M_PI * 3 / 2 / STAR_POINTS);
    vector_t *middle = malloc(sizeof(vector_t));
    assert(middle != NULL);
    middle->x = center.x - (height_2 * cos(M_PI / 2 - M_PI / STAR_POINTS));
    middle->y = center.y + (height_2 * sin(M_PI / 2 - M_PI / STAR_POINTS));
    // add to vec_list
    vec_list_add(poly, top);
    vec_list_add(poly, middle);
    // rotate
    polygon_rotate(poly, -2 * M_PI / (STAR_POINTS), center);
  }

  return poly;
}

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the demo in a created state
 * variable Returns the pointer to this state (This is the state emscripten_main
 * and emscripten_free work with)
 */
state_t *emscripten_init() {
  vector_t min = VEC_ZERO;
  vector_t max = {MAX_WIDTH, MAX_HEIGHT};
  sdl_init(min, max);

  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);

  vector_t center = {MAX_WIDTH / 2, MAX_HEIGHT / 2};
  state->polygon = make_star(center, LENGTH);

  vector_t *vel = malloc(sizeof(vector_t));
  assert(vel != NULL);

  vel->x = DEFAULT_SPEED * cos(DEFAULT_ANGLE);
  vel->y = DEFAULT_SPEED * sin(DEFAULT_ANGLE);
  state->velocity = vel;

  state->rotation = DEFAULT_ROTATION_SPEED;
  state->just_moved = false;
  return state;
}

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed
 */
void emscripten_main(state_t *state) {
  sdl_clear();
  double dt = time_since_last_tick();

  vec_list_t *poly = state->polygon;
  vector_t *vel = state->velocity;
  vector_t vec = {dt * vel->x, dt * vel->y};

  polygon_translate(poly, vec);
  polygon_rotate(poly, dt * state->rotation, polygon_centroid(poly));

  if (!state->just_moved) {
    for (size_t i = 0; i < vec_list_size(poly); i++) {
      vector_t *vector = vec_list_get(poly, i);
      if (vector->y >= MAX_HEIGHT || vector->y <= 0.0) {
        vel->y = -1 * vel->y;
        state->just_moved = true;
        break;
      } else if (vector->x >= MAX_WIDTH || vector->x <= 0.0) {
        vel->x = -1 * vel->x;
        state->just_moved = true;
        break;
      }
    }
  } else {
    state->just_moved = false;
  }

  sdl_draw_polygon(poly, 0, 1, 0);
  sdl_show();
}

/**
 * Frees anything allocated in the demo
 * Should free everything in state as well as state itself.
 */
void emscripten_free(state_t *state) {
  vec_list_t *poly = state->polygon;
  vec_list_free(poly);
  free(state);
}