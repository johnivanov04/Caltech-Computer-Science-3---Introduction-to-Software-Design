#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "body.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"

const double WEDGE_ANGLE = 3.6 * M_PI / 3;
const double INCREMENT_ANGLE = 0.1;
const double RADIUS = 40;
const double BULLET_RADIUS = 10;

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};

const vector_t USER_CENTER = {500, 30};
const vector_t INVADER_BULLET_VEL = {0, -200};
const vector_t PLAYER_CENTER = {50, 0};

const double MASS = 5;
const double BULLET_MASS = 10;
const vector_t BULLET_VELOCITY = {0, 200};

const size_t SPAWN_TIME = 200; // number of frames in between new shapes
const size_t SHOOT_TIME = 2;

const double resting_speed = 300;
const double ACCEL = 100;

const double OUTER_RADIUS = 60;
const double INNER_RADIUS = 25;

const size_t SHIP_NUM_POINTS = 20;
const size_t DEFUALT_LIST = 10;

const rgb_color_t invader_color = (rgb_color_t){0.2, 0.2, 0.3};
const rgb_color_t user_color = (rgb_color_t){0.1, 0.9, 0.2};

// constants to create invaders
const size_t NUM_ROWS = 3;
const size_t Y_START = 390;
const size_t Y_SPACE = 10;
const size_t INVADERS_PER_ROW = 8;
const size_t X_START = 100;
const size_t X_SPACE = 15;

const size_t OFFSET = 3;
const size_t CIRC_NPOINTS = 4;

struct state {
  scene_t *scene;
  size_t invader_count;
  body_t *ship;
  double spawn_time;
  double shoot_time;
};

/** Make a circle-shaped body object.
 *
 * @param center a vector representing the center of the body.
 * @param radius the radius of the circle
 * @param mass the mass of the body
 * @param color the color of the circle
 * @return pointer to the circle-shaped body
 */
body_t *make_bullet(vector_t center, double radius, double mass,
                    rgb_color_t color, void *info) {
  list_t *c = list_init(CIRC_NPOINTS, free);
  for (size_t i = 0; i < CIRC_NPOINTS; i++) {
    double angle = 2 * M_PI * i / CIRC_NPOINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    list_add(c, v);
  }
  return body_init_with_info(c, mass, color, info, NULL);
}

/** Return a list of points representing the invader shape.
 *
 * @param center a vector representing the center of the invader
 * @return list of vectors representing points of invader object.
 */
list_t *make_invader(vector_t center, double wedge) {
  list_t *points = list_init(DEFUALT_LIST, free);
  vector_t *vec = malloc(sizeof(vector_t));
  assert(vec);

  double x;
  double y;
  for (double i = (wedge / 2); i <= (2 * M_PI) - (wedge / 2);
       i += INCREMENT_ANGLE) {
    vector_t *new_vec = malloc(sizeof(vector_t));
    assert(new_vec);

    x = RADIUS * cos(i) + center.x;
    y = RADIUS * sin(i) + center.y;

    *new_vec = (vector_t){x, y};
    list_add(points, new_vec);
  }

  vector_t *new_vec = malloc(sizeof(vector_t));
  assert(new_vec);
  *new_vec = center;
  list_add(points, new_vec);

  return points;
}

list_t *make_ship(double outer_radius, double inner_radius) {
  vector_t center = PLAYER_CENTER;
  center.y += inner_radius;
  list_t *c = list_init(SHIP_NUM_POINTS, free);
  for (size_t i = 0; i < SHIP_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / SHIP_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + outer_radius * cos(angle),
                    center.y + inner_radius * sin(angle)};
    list_add(c, v);
  }
  return c;
}

/**
 * Wrap object around other side of screen display if it reaches any edge of the
 * display.
 *
 * @param body the body object representing an invader
 * @param amount amount to offset in the y-direction
 */
void wrap_edges(body_t *body, double offset) {
  vector_t velocity = body_get_velocity(body);
  vector_t centroid = body_get_centroid(body);
  if (centroid.x + RADIUS > MAX.x && velocity.x > 0) {
    body_set_centroid(body, (vector_t){centroid.x, centroid.y - offset});
    body_set_velocity(body, vec_negate(velocity));
  } else if (centroid.x - RADIUS < MIN.x && velocity.x < 0) {
    body_set_centroid(body, (vector_t){centroid.x, centroid.y - offset});
    body_set_velocity(body, vec_negate(velocity));
  }
}

/**
 * Wrap object around other side of screen display if it reaches any edge of the
 * display.
 *
 * @param body the body object representing the user
 */
void user_wrap_edges(body_t *body) {
  vector_t centroid = body_get_centroid(body);
  if (centroid.x + RADIUS > MAX.x) {
    body_set_centroid(body, (vector_t){MIN.x + RADIUS, centroid.y});
  } else if (centroid.x - RADIUS < MIN.x) {
    body_set_centroid(body, (vector_t){MAX.x - RADIUS, centroid.y});
  }
}

// calculate new locations after wrap-around for invaders
void wrap_invaders(state_t *state) {
  for (size_t i = 0; i < state->invader_count; i++) {
    body_t *invader = scene_get_body(state->scene, i);
    wrap_edges(invader, (RADIUS + 10) * OFFSET);
  }
}

/**
 * Check conditions to see if game is over. Game is over if there are
 * no more invaders (win), the invaders reach the bottom of the screen (loss)
 * or the user is hit by an invader bullet (loss).
 *
 * @param state a pointer to a state object representing the current demo state
 */
bool game_over(state_t *state) {
  if (state->invader_count == 1) {
    return true;
  }
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if ((strcmp(body_get_info(body), "inv_bullet") == 0)) {
      if (find_collision(body, state->ship)) {
        return true;
      }
    }

    if (strcmp(body_get_info(body), "invader") == 0 &&
        body_get_centroid(body).y - RADIUS <= MIN.y) {
      return true;
    }
  }
  return false;
}

/**
 * Move ship on display screen or shoots bullet based based on key pressed.
 *
 * @param key the character of the key pressed
 * @param type event type connected to key
 * @param held_time double value representing the amount of time the key is held
 * down
 * @param scene the scene holding the bodies
 */
void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  if (type == KEY_PRESSED) {
    body_t *user = state->ship;

    switch (key) {
    case LEFT_ARROW:
      body_set_velocity(user, (vector_t){-resting_speed, 0});
      break;
    case RIGHT_ARROW:
      body_set_velocity(user, (vector_t){resting_speed, 0});
      break;
    case SPACE_BAR: {
      vector_t ship = body_get_centroid(user);
      body_t *bullet =
          make_bullet((vector_t){ship.x, ship.y + RADIUS + BULLET_RADIUS},
                      BULLET_RADIUS, BULLET_MASS, user_color, "sh_bullet");
      body_set_velocity(bullet, BULLET_VELOCITY);
      scene_add_body(state->scene, bullet);
      break;
    }
    default:
      break;
    }
  } else {
    if (key == LEFT_ARROW || key == RIGHT_ARROW) {
      body_set_velocity(state->ship, VEC_ZERO);
    }
  }
}

// Checks to see if ship bullet is in contact with invader and then deletes
// both objects if true
void invader_collide(state_t *state) {
  list_t *remove_objects = list_init(DEFUALT_LIST, NULL);
  size_t num_bodies = scene_bodies(state->scene);

  for (size_t i = 0; i < num_bodies; i++) {
    body_t *sh_bullet = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(sh_bullet), "sh_bullet") == 0) {
      for (size_t j = 0; j < num_bodies; j++) {
        body_t *invader = scene_get_body(state->scene, j);
        if (strcmp(body_get_info(invader), "invader") == 0) {
          if (find_collision(sh_bullet, invader)) {
            create_destructive_collision(state->scene, sh_bullet, invader);
            list_add(remove_objects, sh_bullet);
            list_add(remove_objects, invader);
          }
        }
      }
    }
  }

  for (size_t i = 0; i < list_size(remove_objects); i++) {
    body_t *remove = list_get(remove_objects, i);
    if (strcmp(body_get_info(remove), "invader") == 0) {
      state->invader_count--;
    }
  }

  list_free(remove_objects);
}

// initialize the invaders at start of game
void invader_init(state_t *state) {
  for (int i = 0; i < NUM_ROWS; i++) {
    double y = Y_START + i * (RADIUS + Y_SPACE);
    for (int j = 0; j < INVADERS_PER_ROW; j++) {
      double x = X_START + j * (RADIUS * 2 + X_SPACE);

      list_t *invader_points = make_invader((vector_t){x, y}, WEDGE_ANGLE);
      body_t *invader = body_init_with_info(invader_points, MASS, invader_color,
                                            "invader", NULL);

      body_set_velocity(invader, (vector_t){100, 0});
      body_set_rotation(invader, 3 * M_PI / 2);

      scene_add_body(state->scene, invader);
      state->invader_count++;
    }
  }
}

/** Chooses a random invader out of the remaining invaders
 * in the demo to shoot a bullet at the user.
 *
 * @param state a pointer to a state object representing the current demo state
 */
void invader_shoot_bullet(state_t *state) {
  size_t num_enemies = scene_bodies(state->scene);
  list_t *invaders = list_init(DEFUALT_LIST, NULL);

  for (size_t i = 0; i < num_enemies; i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), "invader") == 0) {
      list_add(invaders, body);
    }
  }

  body_t *invader = NULL;
  if (list_size(invaders) > 0) {
    size_t invader_index = rand() % list_size(invaders);
    invader = list_get(invaders, invader_index);

    vector_t invader_position = body_get_centroid(invader);

    body_t *bullet =
        make_bullet((vector_t){invader_position.x, invader_position.y - RADIUS},
                    BULLET_RADIUS, BULLET_MASS, invader_color, "inv_bullet");
    body_set_velocity(bullet, INVADER_BULLET_VEL);
    scene_add_body(state->scene, bullet);
  }
  list_free(invaders);
}

// Gets rid of the invader and ships bullets
void offscreen_delete(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), "inv_bullet") == 0 ||
        strcmp(body_get_info(body), "sh_bullet") == 0) {
      if (body_get_centroid(body).y < MIN.y ||
          body_get_centroid(body).y > MAX.y) {
        scene_remove_body(state->scene, i);
      }
    }
  }
}

state_t *emscripten_init() {
  sdl_init(MIN, MAX);
  state_t *state = malloc(sizeof(state_t));
  assert(state);
  state->scene = scene_init();
  state->ship = body_init_with_info(make_ship(OUTER_RADIUS, INNER_RADIUS), MASS,
                                    user_color, "user", NULL);
  scene_add_body(state->scene, state->ship);
  state->spawn_time = 0;
  state->invader_count = 1;
  invader_init(state);
  return state;
}

bool emscripten_main(state_t *state) {
  sdl_on_key((key_handler_t)on_key);
  wrap_invaders(state);
  double dt = time_since_last_tick();
  state->spawn_time += dt;
  state->shoot_time += dt;
  offscreen_delete(state);
  invader_collide(state);
  if ((size_t)state->spawn_time > SPAWN_TIME) {
    invader_init(state);
    state->spawn_time = 0;
  }
  if ((size_t)state->shoot_time > SHOOT_TIME) {
    invader_shoot_bullet(state);
    state->shoot_time = 0;
  }
  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene, state->ship);
  user_wrap_edges(state->ship);
  if (game_over(state)) {
    return true;
  }
  return false;
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  body_free(state->ship);
  free(state);
}
