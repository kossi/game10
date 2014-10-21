#include "minunit.h"
#include <tetris.h>
#include <dbg.h>

struct Tetromino *t = NULL;
struct State *s = NULL;

void print_grid(){
  printf("Grid:\n");
  for(int i = 0, j; i < T_ROWS; ++i){
    for(j = 0; j < T_COLS; ++j){
      printf("%i",s->grid[i][j]+1);
    }
    printf("\n");
  }
  printf("End\n");
}

char *test_createTetromino(){
  t = tetromino_create(kind_L);
  mu_assert(t != NULL, "Tetromino should be created");
  for(int i = 0; i < 2; ++i)
    mu_assert(t->shape[1][i] > 0, "L-tetromino's first row value should be more than zero");
  mu_assert(t->shape[2][0] > 0, "L-tetromino's 2,2 value should be more than zero");
  mu_assert(t->pos.top == 0, "L-tetromino's position from the top should be 0");
  mu_assert(t->pos.left == 3, "L-tetromino's position from the left should be 3");
  return NULL;
}

char *test_deleteTetromino(){
  bool success = tetromino_delete(t);
  mu_assert(success == true, "Tetromino should be deleted");
  return NULL;
}

char *test_initTetris(){
  s = tetris_init(42l);
  mu_assert(s != NULL, "Tetris should be created");
  mu_assert(s->current->kind == kind_L, "First kind should be kind_L");
  int i = 1;
  for(struct Tetromino *t = s->current; t != NULL; t = t->next){
    i++;
    log_info("Kind: %i",t->kind);
  }
  mu_assert(s->last->kind == kind_T, "Last kind should be kind_T");
  mu_assert(i == 6, "The game should generate 5 tetrominos");
  mu_assert(s->score == 0, "Tetris score should be zero");

  return NULL;
}

char *test_moveRTetris(){
  tetris_move_r(s);
  mu_assert(s->current->pos.left == 4, 
    "L-tetromino's position from the left should be 4");
  for(int i = 0; i < 10; ++i){
    tetris_move_r(s);
  }
  log_info("First place: %i", s->current->pos.left);
  mu_assert(s->current->pos.left == 7, 
    "L-tetromino's position from left after nine move should be 7");
  return NULL;
}

char *test_moveLTetris(){
  tetris_move_l(s);
  mu_assert(s->current->pos.left == 6, 
    "L-tetromino's position from the left should be 6");
  for(int i = 0; i < 10; ++i){
    tetris_move_l(s);
  }
  log_info("First place: %i", s->current->pos.left);
  mu_assert(s->current->pos.left == 0, 
    "L-tetromino's position from left after ten move should be 0");
  return NULL;
}

char *test_moveBlocked(){
  mu_assert(s->current->pos.left == 0, 
    "L-tetromino's position from left after ten move should be 0");
  for(int i = 0, j; i < T_ROWS; ++i){
    for(j = 4; j < T_COLS; ++j){
      s->grid[i][j] = 1;
    }
  }
  tetris_move_r(s);
  mu_assert(s->current->pos.left == 1, 
    "L-tetromino's position from the left should be 1");
  for(int i = 0; i < 4; ++i){
    tetris_move_r(s);
  }
  mu_assert(s->current->pos.left == 1, 
    "L-tetromino's position from left after four move should be still 1");
  return NULL;
}

char *test_drop(){
  tetris_exit(s);
  s = tetris_init(10l);
  mu_assert(s->current->kind == kind_O, "First kind should be kind_O");
  tetris_drop(s);
  //print_grid();
  mu_assert(s->current->kind == kind_J, "Second kind should be kind_J");
  mu_assert(s->grid[20][4] == kind_O, "There should be pieces at the grid");

  return NULL;
}

char *test_rowClear(){
  tetris_exit(s);
  s = tetris_init(1337l);
  for(int i = 7, j; i < T_ROWS; ++i){
    for(j = 3; j < T_COLS; ++j){
      s->grid[i][j] = 1;
    }
  }
  tetris_drop(s);
  for(int i = 0; i < 10; ++i)
    tetris_move_l(s);
  tetris_drop(s);

  mu_assert(s->score == 500, "Score should be 500 after one clear");
  mu_assert(s->grid[21][2] == kind_J, "Leftover from a clear shold be L-kind");
  mu_assert(s->grid[21][0] == -1, "First col at the bottom shuld be clear");

  return NULL;
}

char *test_turn(){
  tetris_exit(s);
  s = tetris_init(9l); 
  tetris_drop(s);
  tetris_turn(s);
  tetris_drop(s);
  for(int i = 16; i < 20; ++i){
    mu_assert(s->grid[i][4] == kind_I, "Grid should have I-kind block there");
    mu_assert(s->grid[i][5] == -1, "Grid shoulbe be clear");
  }
  return NULL;
}

char *test_exitTetris(){
  bool success = tetris_exit(s);
  mu_assert(success == true, "Tetris should be deleted");
  return NULL;
}

char *all_tests(){
  mu_suite_start();

  // Single piece tests
  mu_run_test(test_createTetromino);
  mu_run_test(test_deleteTetromino);

  // Whole state and grid tests
  mu_run_test(test_initTetris);
  mu_run_test(test_moveRTetris);
  mu_run_test(test_moveLTetris);
  mu_run_test(test_moveBlocked);
  mu_run_test(test_drop);
  mu_run_test(test_rowClear);
  mu_run_test(test_turn);
  mu_run_test(test_exitTetris);

  return NULL;
}

RUN_TESTS(all_tests);

