#include "polygon.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct polygon {
  list_t *points;
  vector_t velocity;
  double rotation_speed;
  color_t *color;
};

polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue) {
  polygon_t *polygon = malloc(sizeof(polygon_t));
  assert(polygon);
  polygon->points = points;
  polygon->velocity = initial_velocity;
  polygon->rotation_speed = rotation_speed;
  polygon->color = color_init(red, green, blue);
  return polygon;
}

double polygon_area(polygon_t *polygon) {
  size_t vertices = list_size(polygon->points);
  if (vertices == 0) {
    return 0.0;
  }
  double area = 0.0;

  for (size_t i = 0; i < vertices; i++) {
    vector_t *current = list_get(polygon->points, i);
    vector_t *next = list_get(polygon->points, (i + 1) % vertices);
    area += (current->x * next->y) - (current->y * next->x);
  }
  return fabs(area) / 2.0;
}

vector_t polygon_centroid(polygon_t *polygon) {
  size_t size = list_size(polygon->points);
  if (size < 3) {
    return (vector_t){0, 0};
  }

  double x_value = 0.0;
  double y_value = 0.0;
  for (size_t i = 0; i < size; i++) {
    vector_t *curr = list_get(polygon->points, i);
    vector_t *next = list_get(polygon->points, (i + 1) % size);
    double shoelace = (curr->x * next->y) - (curr->y * next->x);
    x_value += (curr->x + next->x) * shoelace;
    y_value += (curr->y + next->y) * shoelace;
  }
  double area = polygon_area(polygon);
  double sum_x = x_value / (6 * area);
  double sum_y = y_value / (6 * area);
  return (vector_t){sum_x, sum_y};
}

void polygon_translate(polygon_t *polygon, vector_t translation) {
  size_t size = list_size(polygon->points);
  for (size_t i = 0; i < size; i++) {
    vector_t *temp = list_get(polygon->points, i);
    temp->x += translation.x;
    temp->y += translation.y;
  }
}

void polygon_rotate(polygon_t *polygon, double angle, vector_t point) {
  size_t size = list_size(polygon->points);
  for (size_t i = 0; i < size; i++) {
    vector_t *temp = list_get(polygon->points, i);
    temp->x -= point.x;
    temp->y -= point.y;
    vector_t nonpointer = {temp->x, temp->y};
    vector_t rotated = vec_rotate(nonpointer, angle);
    temp->x = rotated.x + point.x;
    temp->y = rotated.y + point.y;
  }
}

list_t *polygon_get_points(polygon_t *polygon) { return polygon->points; }

void polygon_set_velocity(polygon_t *polygon, double v_x, double v_y) {
  polygon->velocity.x = v_x;
  polygon->velocity.y = v_y;
}

void polygon_free(polygon_t *polygon) {
  list_free(polygon->points);
  color_free(polygon->color);
  free(polygon);
}
