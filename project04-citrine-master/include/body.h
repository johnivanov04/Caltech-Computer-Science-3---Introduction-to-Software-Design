#ifndef __BODY_H__
#define __BODY_H__

#include <stdbool.h>

#include "color.h"
#include "list.h"
#include "polygon.h"

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 */
typedef struct body body_t;

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving).
 *   For now, the mass is unused. It will be needed next week for accelerations.
 * @param color the color of the body, used to draw it on the screen
 * @return a pointer to the newly allocated body
 */
body_t *body_init(list_t *shape, double mass, rgb_color_t color);

/**
 * Releases the memory allocated for a body.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_free(body_t *body);

/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape(body_t *body);

/**
 * Gets the current center of mass of a body.
 * While this could be calculated with polygon_centroid(), that becomes too slow
 * when this function is called thousands of times every tick.
 * Instead, the body should store its current centroid.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
vector_t body_get_centroid(body_t *body);

/**
 * Gets the current velocity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's velocity vector
 */
vector_t body_get_velocity(body_t *body);

/**
 * Gets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's color, as an (R, G, B) tuple
 */
rgb_color_t *body_get_color(body_t *body);

/**
 * Sets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param the body's color, as an (R, G, B) tuple
 */
void body_set_color(body_t *body, rgb_color_t *col);

/**
 * Translates a body to a new position.
 * The position is specified by the position of the body's center of mass.
 *
 * @param body a pointer to a body returned from body_init()
 * @param x the body's new centroid
 */
void body_set_centroid(body_t *body, vector_t x);

/**
 * Changes a body's velocity (the time-derivative of its position).
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new velocity
 */
void body_set_velocity(body_t *body, vector_t v);

/**
 * Gets the rotation angle of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's rotation angle in radians
 */
double body_get_rotation(body_t *body);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 */
void body_set_rotation(body_t *body, double angle);

/**
 * Moves a body based on the average of its current velocity and its new
 * velocity (given the total force and impulse acting upon the body) over 
 * a given time interval.
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick(body_t *body, double dt);

/**
 * Gets the mass of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's mass
 */
double body_get_mass(body_t *body);

/**
 * Adds a force to a running total of magnitude of force acting upon body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param force a vector specifying the force to add
 */
void body_add_force(body_t *body, vector_t force);

/**
 * Adds an impulse to a running total of magnitude of impulse acting upon body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param force a vector specifying the impulse to add
 */
void body_add_impulse(body_t *body, vector_t impulse);

/**
 * Mark a body to be removed.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_remove(body_t *body);

/**
 * Return whether or not a body has been marked for removal.
 *
 * @param body a pointer to a body returned from body_init()
 * @return a boolean indicating whether a body is marked for removal
 */
bool body_is_removed(body_t *body);

#endif // #ifndef __BODY_H__