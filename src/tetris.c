#include <stdlib.h>
#include <stdio.h>

#include "dbg.h"
#include "tetris.h"

enum Piece next_piece(struct State *state){
  uint64_t divisor = UINT64_MAX/PIECE_COUNT;
  struct Result r;

  do{
    r = xorshift_next(state->x);
    r.n = r.n / divisor;
    state->x = r.state;
  } while (r.n > PIECE_COUNT);

  return (enum Piece) r.n;
}


struct Tetromino *tetromino_create(enum Piece kind){
  struct Tetromino *t = malloc(sizeof(struct Tetromino));
  if(!t) return NULL;

  memset(t->shape, -1, sizeof(t->shape));
  t->kind = kind;

  switch(kind){
    case kind_J:
      for(int i = 0; i < 3; ++i)
        t->shape[1][i] = (int) t->kind;
      t->shape[2][2] = (int) t->kind;
      break;
    case kind_L:
      for(int i = 0; i < 3; ++i)
        t->shape[1][i] = (int) t->kind;
      t->shape[2][0] = (int) t->kind;
      break;
    case kind_O:
      for(int i = 0; i < 2; ++i){
        t->shape[0][i] = (int) t->kind;
        t->shape[1][i] = (int) t->kind;
      }
      break;
    case kind_S:
      t->shape[1][1] = (int) t->kind; 
      t->shape[1][2] = (int) t->kind;
      t->shape[2][0] = (int) t->kind;    
      t->shape[2][1] = (int) t->kind;       
      break;
    case kind_T:
      for(int i = 0; i < 3; ++i)
        t->shape[1][i] = (int) t->kind;
      t->shape[2][1] = (int) t->kind;
      break;
    case kind_Z:
      for(int i = 1; i < 3; ++i){
        t->shape[i][i] = (int) t->kind; 
        t->shape[i][i-1] = (int) t->kind; 
      }
      break;
    default:
      for(int i = 0; i < 4; ++i)
        t->shape[2][i] = (int) t->kind;
      break;
  }
  struct Pos p = {0, 3};
  t->pos = p;
  t->next = NULL;

  return t;
}

bool tetromino_delete(struct Tetromino *tetromino){
  if(tetromino){
    free(tetromino);
    return true;
  }
  else
    return false;
}

struct State *tetris_init(uint64_t seed){
  struct State *s = malloc(sizeof(struct State));
  if(!s) return NULL;
  
  memset(s->grid, -1, sizeof s->grid);
  s->x = seed; 
  s->score = 0;
  s->party = false;
  struct Tetromino *c = tetromino_create(next_piece(s));
  if(!c){
    tetris_exit(s);
    return NULL;
  }
  s->current = c;
  for(int i = 1; i < T_COUNT; ++i){
    struct Tetromino *t = tetromino_create(next_piece(s));     
    if(!c){
      tetris_exit(s);
      return NULL;
    }
    if(i < T_COUNT-1)
      c->next = t;
    else{
      c->next = t;
      s->last = t;
    }
    c = t;
  }
  
  return s;
}

bool tetris_exit(struct State *state){
  if(state){
    struct Tetromino *nt = NULL;
    if(state->current != NULL){
      for(struct Tetromino *t = state->current; t != NULL; t = nt){
        nt = t->next;
        if(!tetromino_delete(t)) 
          log_err("Error while removing a tetromino from the State");
      }
    }
    free(state);
    return true;
  }
  else
    return false;
}


bool collision_detection(struct State *state){
  for(int i = 0, j, col, row = state->current->pos.top; i < 4; i++, row++){
    j = 0;
    for(col = state->current->pos.left; j < 4; j++, col++){  
      if(state->current->shape[i][j] >= 0){
        if(row >= T_ROWS || row < 0)
          return true;
        else if(col >= T_COLS || col < 0)
          return true; 
        else if(state->grid[row][col] >= 0)
          return true;
      }
    }
  }
  return false; 
};


int row_clearing(struct State *state, int clear){
  for(int i = T_ROWS-1, j, hits; i >= 0; --i){
    for(j = 0, hits = 0; j < T_COLS; ++j){
      if(state->grid[i][j] >= 0) hits++;
    }
    if(hits == T_COLS){
     for(int ni = i-1, nj; ni >= 0; --ni){
        for(nj = 0; nj < T_COLS; ++nj){
          state->grid[ni+1][nj] = state->grid[ni][nj];
        }
      }
      clear++;
      return row_clearing(state, clear);
    }
  }
  return clear;
}

enum TickResult tetris_tick(struct State *state){
  state->current->pos.top++;
  
  if(collision_detection(state)){
    state->current->pos.top--;
    for(int i = 0, j, col, row = state->current->pos.top; i < 4; i++, row++){
      j = 0;
      for(col = state->current->pos.left; j < 4; j++, col++){  
        if(state->current->shape[i][j] >= 0){
          state->grid[row][col] = state->current->kind;
        }    
      }
    }
    
    struct Tetromino *t = state->current;
    
    state->current = t->next;
    tetromino_delete(t);
    
    t = tetromino_create(next_piece(state));
    state->last->next = t;
    state->last = t;
    state->current->pos.top = 0;
    if(collision_detection(state)) 
      return game_over;
    else{
      int clear = row_clearing(state, 0);
      switch(clear){
        case 0:
          break;
        case 1:
          if(state->party) state->score += 1000;
          state->score += 500;
          state->party = true;
          break;
        default:
          if(state->party) state->score += 500*clear;
          state->party = true;
          state->score += clear * 1000;
        break;
      }
      return new_current;
    }
  }else{
    return move_down;
  }
}

enum TickResult tetris_drop(struct State *state){
  enum TickResult r;
  for(r = move_down; r == move_down; r = tetris_tick(state))
    ;

  return r;
}

void rotate_clockwise(struct State *state, int length){
  int old_shape[4][4];

  for(int i = 0; i < 4; ++i){
    for(int j = 0; j < 4; ++j){
      old_shape[i][j] = state->current->shape[i][j];
    }
  }

  for(int i = 0; i < length; ++i){
    for(int j = 0; j < length; ++j){
      state->current->shape[i][j] = old_shape[length-j-1][i];
    }
  }

  if(collision_detection(state)){
    for(int i = 0; i < 4; ++i){
      for(int j = 0; j < 4; ++j){
        state->current->shape[i][j] = old_shape[i][j];
      }
    }
  }
}

void tetris_turn(struct State *state){
  switch(state->current->kind){
    case kind_O:
      break;
    case kind_I:
      rotate_clockwise(state, 4);
      break;
    default:
      rotate_clockwise(state, 3);
      break;
  }
}

void tetris_move_r(struct State *state){
  state->current->pos.left++;
  if(collision_detection(state)) state->current->pos.left--;
}
void tetris_move_l(struct State *state){
  state->current->pos.left--;
  if(collision_detection(state)) state->current->pos.left++;
}
