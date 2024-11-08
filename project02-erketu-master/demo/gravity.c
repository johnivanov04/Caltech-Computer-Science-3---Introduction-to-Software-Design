#include "polygon.h"
#include "sdl_wrapper.h"
#include "state.h"
#include "list.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


const double GRAV_CONST = -0.1;
const double PI = 3.14159265358979323846;
const size_t RADIUS = 70;
const size_t POINTS_MAX = 20;
const size_t POINTS_MIN = 3;

const vector_t VELOCITY_INIT = {3,0};
const double rotate = 0.01;

const vector_t ORIGIN = (vector_t){.x = 0, .y = 0};
const vector_t WINDOW = (vector_t){.x = 1000, .y = 500};
const vector_t START = (vector_t){.x = 100, .y = 430};


typedef struct state {
  list_t *polygon;
  double total_time;
} state_t;

//Random double between 0 and 1
double random_color(){
  return (double)rand() / (double)RAND_MAX;
}

//Random points for star ensures that star points are even number
size_t random_points(){
  size_t points = (size_t)(rand() % (POINTS_MAX-POINTS_MIN+1) + POINTS_MIN);
  if (points % 2 != 0){
    points += 1;
  }
  return points;
}



polygon_t *make_polygon(size_t radius, size_t points, size_t center_x, size_t center_y, double red, double green, double blue){
  list_t *point = list_init(points);
  for (size_t i = 0; i < points; i++){
    vector_t *vertex = malloc(sizeof(vector_t));
    double angle = ((2*PI) / (points)) * i;

  if (i%2 ==0){
    vertex->x = center_x + radius * cos(angle);
    vertex->y = center_y + radius * sin(angle);
  }else{
    vertex->x = center_x + radius / 2 * cos(angle);
    vertex->y = center_y + radius / 2 * sin(angle);
  }

    
    list_add(point, vertex);
  }
  polygon_t *star = polygon_init(point, VELOCITY_INIT, rotate, red, green, blue);
  return star;
}


void update_star(polygon_t *star){
  vector_t centroid = polygon_centroid(star);
  list_t *points = polygon_get_points(star);
  double velocity_x = polygon_get_velocity_x(star);
  double velocity_y = polygon_get_velocity_y(star) + GRAV_CONST;

  polygon_set_velocity(star, velocity_x, velocity_y);
  polygon_rotate(star, rotate, centroid);

  vector_t vel = {velocity_x, velocity_y};
  polygon_translate(star, vel);
  for (size_t i = 0; i < list_size(polygon_get_points(star)); i++){
    vector_t *position = list_get(points, i);
    if (position->y < 0 || position->y > WINDOW.y){
      vel.y *= -0.9;
      polygon_set_velocity(star, velocity_x, vel.y);
      break;
    }
  }
}


bool star_out(polygon_t *star){
  list_t *points = polygon_get_points(star);
  for (size_t i = 0; i < list_size(polygon_get_points(star)); i++){
    vector_t *pos = list_get(points, i);
    if (pos->x > 0 && pos->x < WINDOW.x && pos->y > 0 && pos->y < WINDOW.y){
      return false;
    }
  }
  return true;
}


state_t *emscripten_init() {
  sdl_init(ORIGIN, WINDOW);
  state_t *state = malloc(sizeof(state_t));
  state->total_time = 0;
  list_t *polygon = list_init(1);

  list_add(polygon, make_polygon(RADIUS, random_points(), START.x, START.y, random_color(), random_color(), random_color()));
  state->polygon = polygon;
  return state;
}


void emscripten_main(state_t *state) {
  sdl_clear();
  for (size_t i = 0; i < list_size(state->polygon); i++){
    polygon_t *poly = list_get(state->polygon, i);
    update_star(poly);
    sdl_draw_polygon(poly, color_get_red(polygon_get_color(poly)), color_get_green(polygon_get_color(poly)), color_get_blue(polygon_get_color(poly)));
    if (star_out(poly)){
      polygon_free(poly);
      list_remove(state->polygon, i);
    }
  }
  
  double diff_time = time_since_last_tick();
  state->total_time += diff_time;
  if (state->total_time > 1){
    state->total_time = 0;
    polygon_t *poly = make_polygon(RADIUS, random_points(), START.x, START.y, random_color(), random_color(), random_color());
    list_add(state->polygon, poly);
  }
  sdl_show();
}


void emscripten_free(state_t *state) {
  for (size_t i = 0; i < list_size(state->polygon); i++){
    polygon_free(list_get(state->polygon, i));
  }
  list_free(state->polygon);
  free(state);
}