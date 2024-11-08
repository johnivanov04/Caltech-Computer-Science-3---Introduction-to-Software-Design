#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "forces.h"

const size_t BODIES_INIT = 0;
const size_t CAPACITY_INIT = 10;

struct scene {
  size_t num_bodies;
  list_t *bodies;
  size_t capacity;
  list_t *force_creator_list;
};

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene != NULL);

  scene->num_bodies = BODIES_INIT;
  scene->capacity = CAPACITY_INIT;
  scene->bodies = list_init(scene->capacity, (free_func_t)body_free);
  scene->force_creator_list  = list_init(scene->capacity, (free_func_t)forcer_free);
  return scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->force_creator_list);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return scene->num_bodies; }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
  scene->num_bodies++;
}

void scene_remove_body(scene_t *scene, size_t index) {
  list_remove(scene->bodies, index);
  scene->num_bodies--;
}

void scene_add_force_creator(scene_t *scene, force_creator_t creator, void *aux){
  forcer_t *force = forcer_init(creator, aux);
  list_add(scene->force_creator_list, force);
}

void scene_tick(scene_t *scene, double dt) {
  for (size_t i = 0; i < list_size(scene->force_creator_list); i++) {
    forcer_t *force = list_get(scene->force_creator_list, i);
    if (force && force->creator) {
      force->creator(force->aux);
    }
  }
  for (size_t i = 0; i < scene->num_bodies; i++) {
    body_tick(list_get(scene->bodies, i), dt);
  }
}
