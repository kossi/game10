#include "render.h"

#include "dbg.h"


void grid(){
  glColor3f(0.f, 0.f, 0.f);

  glBegin(GL_LINES); {
    for(int i = 20; i <= rows; i += 10){
      glVertex2f(0.f, i);
      glVertex2f((float) cols, i);
    }
    for(int i = 0; i <= cols; i += 10){
      glVertex2f((float) i, 20.f);
      glVertex2f(i, (float) rows);
    }
  } glEnd();
}

void box(float y, float x, float s){
  float size_max = 10.f - s;
  glBegin(GL_QUADS); {
      glVertex2f(y+s, x+size_max);
      glVertex2f(y+size_max, x+size_max);
      glVertex2f(y+size_max, x+s);
      glVertex2f(y+s, x+s);
  } glEnd();
}

void block(int r, int c, enum Piece kind, float i, bool party){
  float y = (float) r*10;
  float x = (float) c*10;
  if(i >= 0.f)
    glColor3f(i, i, i); 
  else
    glColor3f(0.f, 0.f, 0.f);

  switch(kind){
    case kind_I:
      box(x, y, 1.0f);
      break;
    case kind_O:
      box(x, y, 1.5f);
      break;
    case kind_L:
     glBegin(GL_TRIANGLES); {
        glVertex2f(x+1.f, y+9.f);
        glVertex2f(x+9.f, y+9.f);
        glVertex2f(x+1.f, y+1.f); 
      } glEnd();
      break;
    case kind_J:
      glBegin(GL_TRIANGLES); {
        glVertex2f(x+9.f, y+1.f);
        glVertex2f(x+1.f, y+1.f);
        glVertex2f(x+9.f, y+9.f); 
      } glEnd();
      break;
    case kind_S:
        glBegin(GL_TRIANGLES); {
          glVertex2f(x+1.f, y+1.f);
          glVertex2f(x+8.f, y+1.f);
          glVertex2f(x+1.f, y+8.f);

          if(party) glColor3f(1.f, 0.f, 0.f);
          glVertex2f(x+9.f, y+2.f);
          glVertex2f(x+2.f, y+9.f);
          glVertex2f(x+9.f, y+9.f); 
        } glEnd();
      break;    
    case kind_Z:
        glBegin(GL_TRIANGLES); {
          glVertex2f(x+1.f, y+1.f);
          glVertex2f(x+1.f, y+9.f);
          glVertex2f(x+9.f, y+9.f);
   
          if(party) glColor3f(.0f, .1f, .8f);
          glVertex2f(x+9.f, y+1.f);
          glVertex2f(x+9.f, y+9.f);
          glVertex2f(x+1.f, y+1.f);
       } glEnd();
      break;     
    case kind_T:
      if(party) glColor3f(.0f, 0.f, 1.f);
        box(x, y, 2.f);
      break;
  }
}

void draw(GLFWwindow *window, struct State *state){
  
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  
  glViewport(0.0f, 0.0f, width, height);
  glClearColor(1.f,1.f,1.f,1.f);
  
  glClear(GL_COLOR_BUFFER_BIT);
 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glOrtho(-10.f, ((float) cols+10),((float) rows+10), 10.f,-1.f,1.f);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

  
  grid();

  
  for(int i = 0; i < T_ROWS; ++i){
    for(int j = 0; j < T_COLS; ++j){
      if(state->grid[i][j] >= 0){
        block(i, j, state->grid[i][j], -1.0f, state->party);
      }
    }
  }
  for(int i = 0; i < 4; ++i){
    for(int j = 0; j < 4; ++j){
      if(state->current->shape[i][j] >= 0){
        float black = 0.3f;
        if(state->current->pos.top+i < 2) 
          black = 0.8f;
        block(state->current->pos.top+i,state->current->pos.left+j, 
          state->current->kind, black, false);
      }
    }
  }
}
