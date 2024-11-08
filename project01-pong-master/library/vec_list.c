#include "vec_list.h"
#include <assert.h>
#include <stdlib.h>

typedef struct vec_list {
  vector_t **vectors;
  size_t length;
  size_t capacity;
} vec_list_t;

vec_list_t *vec_list_init(size_t initial_size) {
  vec_list_t *list = malloc(sizeof(vec_list_t));
  assert(list);
  list->vectors = calloc(initial_size, sizeof(vector_t *));
  list->capacity = initial_size;
  list->length = 0;
  return list;
}

void vec_list_free(vec_list_t *list) {
  for (size_t i = 0; i < list->capacity; i++) {
    free(list->vectors[i]);
  }
  free(list->vectors);
  free(list);
}

size_t vec_list_size(vec_list_t *list) { return list->length; }

vector_t *vec_list_get(vec_list_t *list, size_t index) {
  assert(index < list->length);
  return list->vectors[index];
}

void vec_list_add(vec_list_t *list, vector_t *value) {
  assert(list->length + 1 <= list->capacity);
  assert(value);
  list->vectors[list->length] = value;
  list->length++;
}

vector_t *vec_list_remove(vec_list_t *list) {
  assert(list->length != 0);
  vector_t *temp = list->vectors[list->length - 1];
  list->vectors[list->length - 1] = NULL;
  list->length--;
  return temp;
}