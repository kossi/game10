#include <stdlib.h>
#include <stdio.h>

#include "game.h"
#include "render.h"
#include "tetris.h"

#include "dbg.h"

#define UNUSED(x) (void)(x)

static void error_callback(int error, const char*description){
  log_err("Error code: %i Message: %s", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
  UNUSED(scancode);
  UNUSED(mods);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  else if(action > 0){
    struct State *state = (struct State*) glfwGetWindowUserPointer(window);
    switch(key){
      case GLFW_KEY_RIGHT:
        tetris_move_r(state); 
        break;
      case GLFW_KEY_LEFT:
        tetris_move_l(state);
        break;
      case GLFW_KEY_DOWN:
        tetris_tick(state);
        break;
      case GLFW_KEY_UP:
        tetris_turn(state);
        break;
      case GLFW_KEY_SPACE:
        tetris_drop(state);
        break;
    }
  }
}

static void resize_callback(GLFWwindow *window, int width, int height){
 if(width > MIN_RES_W || height > MIN_RES_H){ 
    glfwSetWindowSize(window, MIN_RES_W, MIN_RES_H);
  }else{
    glfwSetWindowSize(window, width, height);
  }
}


GLFWwindow* init(){
  GLFWwindow *window;
  glfwSetErrorCallback(error_callback);
   
  if(!glfwInit()) return NULL;
                
  
  window = glfwCreateWindow(MIN_RES_W, MIN_RES_H, "", NULL, NULL);
    
  if (!window){
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetWindowSizeCallback(window, resize_callback);

  return window;

}

int start(long seed){

  GLFWwindow *window = init();

  struct State *state = tetris_init(seed); 

  if(!window || !state) exit(EXIT_FAILURE);

  glfwSetWindowUserPointer(window, state);
 
  double time = glfwGetTime(), last_time = glfwGetTime();
  int frames = 0;
  int ptick = 0;
  
  while (!glfwWindowShouldClose(window)){
    time = glfwGetTime();
    glfwPollEvents();
    if(time - last_time >= 0.5f){
      enum TickResult res = tetris_tick(state);
      if(res == game_over)   
       glfwSetWindowShouldClose(window, GL_TRUE);
      else if(res == move_down && state->party)
        ptick++;
      
      char title[256];
      double diffms = (time - last_time)*1000;
      snprintf(title, 255,"Score: %i FPS: %3.0f LAT: %3.2fms",
      state->score,(double) frames*2, diffms/frames);

      glfwSetWindowTitle (window, title);

      last_time = glfwGetTime();
      frames = 0;
      if(ptick > 6){
        state->party = false;
        ptick = 0;
      }
    }

    draw(window, state);
    glfwSwapBuffers(window);

    frames++;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  int score = state->score;
  tetris_exit(state);
  return score;
}
