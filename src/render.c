#include "render.h"


void grid(){
  glColor3f(0.f, 0.f, 0.f);
  glLoadIdentity();
  glTranslatef(10.f, 10.f, 0.f);

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

void box(float s){
  float size_max = 10.f - s;
  glBegin(GL_QUADS); {
      glVertex2f(s, size_max);
      glVertex2f(size_max, size_max);
      glVertex2f(size_max, s);
      glVertex2f(s, s);
  } glEnd();
}

void block(int r, int c, enum Piece kind, float i, bool party){
  float y = (float) 10+r*10;
  float x = (float) 10+c*10;
  glLoadIdentity();
  glTranslatef(x,y, 0.f);

  if(i >= 0.f)
    glColor3f(i, i, i); 
  else
    glColor3f(0.f, 0.f, 0.f);

  switch(kind){
    case kind_I:
      box(1.0f);
      break;
    case kind_O:
      box(1.5f);
      break;
    case kind_L:
     glBegin(GL_TRIANGLES); {
        glVertex2f(1.f, 9.f);
        glVertex2f(9.f, 9.f);
        glVertex2f(1.f, 1.f);
      } glEnd();
      break;
    case kind_J:
      glBegin(GL_TRIANGLES); {
        glVertex2f(9.f, 1.f);
        glVertex2f(1.f, 1.f);
        glVertex2f(9.f, 9.f);
      } glEnd();
      break;
    case kind_S:
        glBegin(GL_TRIANGLES); {
          glVertex2f(1.f, 1.f);
          glVertex2f(8.f, 1.f);
          glVertex2f(1.f, 8.f);

          if(party) glColor3f(1.f, 0.f, 0.f);
          glVertex2f(9.f, 2.f);
          glVertex2f(2.f, 9.f);
          glVertex2f(9.f, 9.f);
        } glEnd();
      break;    
    case kind_Z:
        glBegin(GL_TRIANGLES); {
          glVertex2f(1.f, 1.f);
          glVertex2f(1.f, 9.f);
          glVertex2f(9.f, 9.f);
   
          if(party) glColor3f(.0f, .1f, .8f);
          glVertex2f(9.f, 1.f);
          glVertex2f(9.f, 9.f);
          glVertex2f(1.f, 1.f);
       } glEnd();
      break;     
    case kind_T:
      if(party) glColor3f(1.f, 1.f, .0f);
      box(2.f);
      break;
  }
}

void draw(GLFWwindow *window, struct State *state){
  
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  
  glViewport(0, 0, width, height);
  glClearColor(1.f,1.f,1.f,1.f);
  
  glClear(GL_COLOR_BUFFER_BIT);
 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glOrtho(0.f, ((float) cols+20),((float) rows+30), 0.f,-1.f,1.f);
  
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

  grid();
  
  for(int i = 0; i < T_ROWS; ++i){
    for(int j = 0; j < T_COLS; ++j){
      if(state->grid[i][j] >= 0){
        block(i, j, (enum Piece) state->grid[i][j], -1.0f, state->party);
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
