#include "body.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

void test_body_init() {
  vector_t v[] = {{1, 1}, {2, 1}, {2, 2}, {1, 2}};
  const size_t VERTICES = sizeof(v) / sizeof(*v);
  list_t *shape = list_init(1, free);
  for (size_t i = 0; i < VERTICES; i++) {
    vector_t *list_v = malloc(sizeof(*list_v));
    *list_v = v[i];
    list_add(shape, list_v);
  }
  rgb_color_t color = {0, 0.5, 1};
  body_t *body = body_init(shape, 3, color);
  list_t *shape2 = body_get_shape(body);
  assert(list_size(shape2) == VERTICES);
  for (size_t i = 0; i < VERTICES; i++) {
    assert(vec_isclose(*(vector_t *)list_get(shape2, i), v[i]));
  }
  list_free(shape2);
  assert(vec_isclose(body_get_centroid(body), (vector_t){1.5, 1.5}));
  assert(vec_equal(body_get_velocity(body), VEC_ZERO));

  assert(body_get_color(body)->r == color.r);
  assert(body_get_color(body)->g == color.g);
  assert(body_get_color(body)->b == color.b);

  body_free(body);
}

void test_body_setters() {
  list_t *shape = list_init(3, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){+1, 0};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){0, +1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, 0};
  list_add(shape, v);
  body_t *body = body_init(shape, 1, (rgb_color_t){0, 0, 0});
  body_set_velocity(body, (vector_t){+5, -5});
  assert(vec_equal(body_get_velocity(body), (vector_t){+5, -5}));
  assert(vec_isclose(body_get_centroid(body), (vector_t){0, 1.0 / 3.0}));
  body_set_centroid(body, (vector_t){1, 2});
  assert(vec_isclose(body_get_centroid(body), (vector_t){1, 2}));
  shape = body_get_shape(body);
  assert(list_size(shape) == 3);
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 0), (vector_t){2, 5.0 / 3.0}));
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 1), (vector_t){1, 8.0 / 3.0}));
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 2), (vector_t){0, 5.0 / 3.0}));
  list_free(shape);
  body_set_rotation(body, M_PI / 2);
  assert(vec_isclose(body_get_centroid(body), (vector_t){1, 2}));
  shape = body_get_shape(body);
  assert(list_size(shape) == 3);
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 0), (vector_t){4.0 / 3.0, 3}));
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 1), (vector_t){1.0 / 3.0, 2}));
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 2), (vector_t){4.0 / 3.0, 1}));
  list_free(shape);
  body_set_centroid(body, (vector_t){3, 4});
  assert(vec_isclose(body_get_centroid(body), (vector_t){3, 4}));
  shape = body_get_shape(body);
  assert(list_size(shape) == 3);
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 0), (vector_t){10.0 / 3.0, 5}));
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 1), (vector_t){7.0 / 3.0, 4}));
  assert(
      vec_isclose(*(vector_t *)list_get(shape, 2), (vector_t){10.0 / 3.0, 3}));
  list_free(shape);
  body_free(body);
}

void test_body_tick() {
  const vector_t A = {1, 2};
  const double DT = 1e-6;
  const int STEPS = 1000000;
  list_t *shape = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  v->x = v->y = -1;
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  v->x = v->y = +1;
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, +1};
  list_add(shape, v);
  body_t *body = body_init(shape, 1, (rgb_color_t){0, 0, 0});

  // Apply constant acceleration and ensure position is (a / 2) * t ** 2
  for (int i = 0; i < STEPS; i++) {
    double t = i * DT;
    assert(vec_isclose(body_get_centroid(body), vec_multiply(t * t / 2, A)));
    body_set_velocity(body, vec_multiply(t + DT / 2, A));
    body_tick(body, DT);
  }
  double t = STEPS * DT;
  vector_t new_x = vec_multiply(t * t / 2, A);
  shape = body_get_shape(body);
  assert(vec_isclose(*(vector_t *)list_get(shape, 0),
                     vec_add((vector_t){-1, -1}, new_x)));
  assert(vec_isclose(*(vector_t *)list_get(shape, 1),
                     vec_add((vector_t){+1, -1}, new_x)));
  assert(vec_isclose(*(vector_t *)list_get(shape, 2),
                     vec_add((vector_t){+1, +1}, new_x)));
  assert(vec_isclose(*(vector_t *)list_get(shape, 3),
                     vec_add((vector_t){-1, +1}, new_x)));
  list_free(shape);
  body_free(body);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_body_init)
  DO_TEST(test_body_setters)
  DO_TEST(test_body_tick)

  puts("body_test PASS");
}
