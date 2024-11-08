#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "scene.h"
#include "sdl_wrapper.h"
#include "state.h"

// pacman info
const vector_t START_LOC_PACMAN = {500, 250};
const size_t PAC_MASS = 100;
const double WEDGE_ANGLE = M_PI / 3; // angle of pacman's mouth
const double INCREMENT_ANGLE = 0.1;  // angle between points on pacman shape
const double PAC_RADIUS = 60;
const double BALL_RADIUS = 10; // radius of balls (pacman's food)
const int CIRC_NPOINTS = 20;   // points to draw circle
const rgb_color_t PAC_COLOR = {1, 1, 0};
const double ACCELERATION = 200.0;
const vector_t VEC_ZEROS = {0, 0};
const double INITIAL_V = 150;

// pellet info
const size_t PELLET_MASS = 1;
const double SPAWN_RATE = 2.0;

// scene info
const size_t BODIES_INIT = 0;
const size_t CAPACITY_INIT = 10;
const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const int WINDOW_W = 1000;
const int WINDOW_H = 500;

/*
NOTE: Add any other fields in state you need for your demo
*/
struct state {
  scene_t *scene;
  double time;
};

/** Make a circle-shaped body object.
 *
 * @param center a vector representing the center of the body.
 * @param radius the radius of the circle
 * @param mass the mass of the body
 * @param color the color of the circle
 * @return pointer to the circle-shaped body
 */
body_t *make_circle(vector_t center, double radius, double mass,
                    rgb_color_t color) {
  list_t *c = list_init(CIRC_NPOINTS, free);

  for (size_t i = 0; i < CIRC_NPOINTS; i++) {
    double angle = 2 * M_PI * i / CIRC_NPOINTS;

    vector_t *v = malloc(sizeof(*v));
    assert(v);

    vector_t unit = {cos(angle), sin(angle)};
    *v = vec_add(vec_multiply(radius, unit), center);

    list_add(c, v);
  }

  return body_init(c, mass, color);
}

/** Return a list of points representing the pacman shape.
 *
 * @param center a vector representing the center of the pacman
 * @return list of vectors representing points of pacman object.
 */
list_t *get_pacman(vector_t center) {
  list_t *points = list_init(BALL_RADIUS, free);
  vector_t *vec = malloc(sizeof(vector_t));
  assert(vec);

  for (double i = (WEDGE_ANGLE / 2); i <= (2 * M_PI) - (WEDGE_ANGLE / 2);
       i += INCREMENT_ANGLE) {
    vector_t *new_vec = malloc(sizeof(vector_t));
    assert(new_vec);

    vector_t angle = {cos(i), sin(i)};
    *new_vec = vec_add(vec_multiply(PAC_RADIUS, angle), center);

    list_add(points, new_vec);
  }

  vector_t *new_vec = malloc(sizeof(vector_t));
  assert(new_vec);
  *new_vec = center;

  list_add(points, new_vec);
  return points;
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  body_t *pacman = scene_get_body(state->scene, 0);
  vector_t velocity = {0, 0};
  if (type == KEY_PRESSED) {
    switch (key) {
    case UP_ARROW:
      body_set_rotation(pacman, M_PI / 2.0);
      velocity.y = INITIAL_V + (held_time * ACCELERATION);
      body_set_velocity(pacman, velocity);
      break;
    case DOWN_ARROW:
      body_set_rotation(pacman, -1 * M_PI / 2.0);
      velocity.y = (-1 * INITIAL_V) + (-1 * held_time * ACCELERATION);
      body_set_velocity(pacman, velocity);
      break;
    case RIGHT_ARROW:
      body_set_rotation(pacman, 0);
      velocity.x = INITIAL_V + (held_time * ACCELERATION);
      body_set_velocity(pacman, velocity);
      break;
    case LEFT_ARROW:
      body_set_rotation(pacman, -1 * M_PI);
      velocity.x = (-1 * INITIAL_V) + (-1 * held_time * ACCELERATION);
      body_set_velocity(pacman, velocity);
      break;
    }
  } else if (type == KEY_RELEASED) {
    body_set_velocity(pacman, VEC_ZEROS);
  }
}

void wrap_around(body_t *pacman) {
  // wrap for left and right of screen
  if (body_get_centroid(pacman).x - PAC_RADIUS > WINDOW_W) {
    double new_x = body_get_centroid(pacman).x - WINDOW_W;
    vector_t new_centroid = {new_x, body_get_centroid(pacman).y};
    body_set_centroid(pacman, new_centroid);
  } else if (body_get_centroid(pacman).x + PAC_RADIUS < 0) {
    double new_x = WINDOW_W - body_get_centroid(pacman).x;
    vector_t new_centroid = {new_x, body_get_centroid(pacman).y};
    body_set_centroid(pacman, new_centroid);
  }
  // wrap for top and bottom of screen
  if (body_get_centroid(pacman).y - PAC_RADIUS > WINDOW_H) {
    double new_y = body_get_centroid(pacman).y - WINDOW_H;
    vector_t new_centroid = {body_get_centroid(pacman).x, new_y};
    body_set_centroid(pacman, new_centroid);
  } else if (body_get_centroid(pacman).y + PAC_RADIUS < 0) {
    double new_y = WINDOW_H - body_get_centroid(pacman).y;
    vector_t new_centroid = {body_get_centroid(pacman).x, new_y};
    body_set_centroid(pacman, new_centroid);
  }
}

void eat(scene_t *scene) {
  body_t *pacman = scene_get_body(scene, 0);
  if (scene_bodies(scene) > 1) {
    for (size_t i = 1; i < scene_bodies(scene); i++) {
      body_t *pellet = scene_get_body(scene, i);
      vector_t center = body_get_centroid(pacman);
      double pellet_x = (body_get_centroid(pellet)).x;
      double pellet_y = (body_get_centroid(pellet)).y;
      if (PAC_RADIUS / 2 > fabs(pellet_x - center.x) &&
          PAC_RADIUS / 2 > fabs(pellet_y - center.y)) {
        scene_remove_body(scene, i);
      }
    }
  }
}

vector_t spawn() {
  vector_t coord = {(rand() % WINDOW_W), (rand() % WINDOW_H)};
  return coord;
}

state_t *emscripten_init() {
  // initializing the state and scene
  sdl_init(MIN, MAX);
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->time = 0;

  // making pacman
  list_t *pacman_shape = get_pacman(START_LOC_PACMAN);
  body_t *pacman = body_init(pacman_shape, PAC_MASS, PAC_COLOR);
  scene_add_body(state->scene, pacman);

  // create pellets in random locations
  for (size_t i = 0; i < CAPACITY_INIT; i++) {
    vector_t pellet_loc = {(rand() % WINDOW_W), (rand() % WINDOW_H)};
    body_t *pellet =
        make_circle(pellet_loc, BALL_RADIUS, PELLET_MASS, PAC_COLOR);
    scene_add_body(state->scene, pellet);
  }
  return state;
}

void emscripten_main(state_t *state) {
  sdl_on_key((key_handler_t)on_key);
  body_t *pacman = scene_get_body(state->scene, 0);

  double dt = time_since_last_tick();
  state->time += dt;
  scene_tick(state->scene, dt);
  wrap_around(pacman);
  eat(state->scene);
  if (state->time > SPAWN_RATE) {
    vector_t loc = spawn();
    body_t *circle = make_circle(loc, BALL_RADIUS, PELLET_MASS, PAC_COLOR);
    scene_add_body(state->scene, circle);
    state->time = 0;
  }

  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
