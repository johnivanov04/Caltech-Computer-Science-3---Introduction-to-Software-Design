#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "body.h"

const double STARTING_ROT = 0.0;

struct body {
  polygon_t *poly;
  double mass;
};

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);

  vector_t starting_vel = {0, 0};
  body->poly = polygon_init(shape, starting_vel, STARTING_ROT, color.r, color.g,
                            color.b);
  body->mass = mass;
  return body;
}

void body_free(body_t *body) {
  if (body != NULL) {
    polygon_free(body->poly);
    free(body);
    body = NULL;
  }
}

list_t *body_get_shape(body_t *body) {
  list_t *polygon_points = polygon_get_points(body->poly);
  list_t *ret_version = list_init(list_size(polygon_points), (free_func_t)free);
  for (size_t i = 0; i < list_size(polygon_points); i++) {
    vector_t *prev = list_get(polygon_points, i);
    vector_t *copy_over = malloc(sizeof(vector_t));

    *copy_over = *prev;
    list_add(ret_version, copy_over);
  }
  return ret_version;
}

vector_t body_get_centroid(body_t *body) {
  return polygon_get_center(body->poly);
}

vector_t body_get_velocity(body_t *body) {
  double vx = polygon_get_velocity_x(body->poly);
  double vy = polygon_get_velocity_y(body->poly);

  return (vector_t){vx, vy};
}

rgb_color_t *body_get_color(body_t *body) {
  return polygon_get_color(body->poly);
}

void body_set_color(body_t *body, rgb_color_t *col) {
  polygon_set_color(body->poly, col);
}

void body_set_centroid(body_t *body, vector_t x) {
  polygon_set_center(body->poly, x);
}

void body_set_velocity(body_t *body, vector_t v) {
  polygon_set_velocity(body->poly, v.x, v.y);
}

double body_get_rotation(body_t *body) {

  return polygon_get_rotation(body->poly);
}

void body_set_rotation(body_t *body, double angle) {
  polygon_set_rotation(body->poly, angle);
}

void body_tick(body_t *body, double dt) {
  vector_t current_velocity = body_get_velocity(body);
  vector_t movement = {current_velocity.x * dt, current_velocity.y * dt};

  polygon_translate(body->poly, movement);
}

double body_get_mass(body_t *body) { return body->mass; }
