#include "list.h"
#include <math.h>

/**
 * Implement list methods here!
 * You should use the code for vec_list.c as a starting point and modify it to
 * fit the new list_t interface.
 */
#include <assert.h>
#include <stdlib.h>

typedef struct list {

  size_t length;
  size_t capacity;
  void **data;

} list_t;

list_t *list_init(size_t initial_capacity) {
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);
  list->capacity = initial_capacity;
  list->length = 0;
  list->data = malloc(initial_capacity * sizeof(void *));

  return list;
}

void list_free(list_t *list) {
  for (size_t i = 0; i < list_size(list); i++) {
    free(list->data[i]);
  }
  free(list->data);
  free(list);
}

size_t list_size(list_t *list) { return list->length; }

void *list_get(list_t *list, size_t index) {

  assert(index < list_size(list));
  assert(index >= 0);
  void *ret = list->data[index];
  return ret;
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);
  if (list->length >= list->capacity) {
    // Resize the list
    void **new_data = realloc(list->data, list->capacity * 2 * sizeof(void *));
    assert(new_data != NULL);

    list->capacity *= 2;
    list->data = new_data;
  }

  list->data[list->length] = value;
  list->length++;
}

void *list_remove(list_t *list, size_t index) {
  assert(index < list->length);
  void *removed_value = list->data[index];

  for (size_t i = index; i < list->length - 1; i++) {
    list->data[i] = list->data[i + 1];
  }
  list->length--;
  return removed_value;
}
