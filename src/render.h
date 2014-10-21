#ifndef game10_render_h
#define game10_render_h

#include <GLFW/glfw3.h>
#include "tetris.h"

#define MIN_RES_H 560
#define MIN_RES_W 340

// The origin of the tetris grid is at the top-left corner
static const int rows = T_ROWS*10; 
static const int cols = T_COLS*10;

void draw(GLFWwindow *window, struct State *state);

#endif
