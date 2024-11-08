#include "polygon.h"
#include <math.h>
#include <stdio.h>

double polygon_area(vec_list_t *polygon) {
  double area = 0.0;
  size_t vertices = vec_list_size(polygon);
  for (size_t i = 0; i < vertices; i++) {
    vector_t *current = vec_list_get(polygon, i);
    vector_t *next = vec_list_get(polygon, (i + 1) % vertices);
    area += (current->x * next->y) - (current->y * next->x);
  }
  return fabs(area) / 2.0;
}

vector_t polygon_centroid(vec_list_t *polygon) {
  size_t size = vec_list_size(polygon);
  double x_value = 0.0;
  double y_value = 0.0;
  for (size_t i = 0; i < size; i++) {
    vector_t *curr = vec_list_get(polygon, i);
    vector_t *next = vec_list_get(polygon, (i + 1) % size);
    double shoelace = (curr->x * next->y) - (curr->y * next->x);
    x_value += (curr->x + next->x) * shoelace;
    y_value += (curr->y + next->y) * shoelace;
  }
  double area = polygon_area(polygon);
  double sum_x = x_value / (6 * area);
  double sum_y = y_value / (6 * area);
  vector_t ret = {sum_x, sum_y};
  return ret;
}

void polygon_translate(vec_list_t *polygon, vector_t translation) {
  size_t size = vec_list_size(polygon);
  for (size_t i = 0; i < size; i++) {
    vector_t *temp = vec_list_get(polygon, i);
    temp->x += translation.x;
    temp->y += translation.y;
  }
}

void polygon_rotate(vec_list_t *polygon, double angle, vector_t point) {
  size_t size = vec_list_size(polygon);
  for (size_t i = 0; i < size; i++) {
    vector_t *temp = vec_list_get(polygon, i);
    temp->x -= point.x;
    temp->y -= point.y;
    vector_t nonpointer = {temp->x, temp->y};
    temp->x = vec_rotate(nonpointer, angle).x;
    temp->y = vec_rotate(nonpointer, angle).y;
    temp->x += point.x;
    temp->y += point.y;
  }
}