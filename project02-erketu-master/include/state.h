
#include <stdio.h>
#include <stdlib.h>
#include "sdl_wrapper.h"
#include "polygon.h"
#include <math.h>

/**
 * Stores the demo state
 * Use this to store any variable needed every 'tick' of your demo
 * ADDED SECOND TEXTURE TO THE STRUCT & ADDED A CURRENT TEXTURE TRACKING VAR & SAWP TIME TRACKING VAR**********
 */ 
typedef struct state state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the demo in a created state variable
 * Returns the pointer to this state
 * (This is the state emscripten_main and emscripten_free work with)
 */
state_t *emscripten_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time that has passed
 */ 
void emscripten_main(state_t *state);

/**
 * Frees anything allocated in the demo
 * Should free everything in state as well as state itself. 
 */ 
void emscripten_free(state_t *state);
