#ifndef game10_tetris_h
#define game10_tetris_h

#include <stdbool.h>
#include <stdint.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#define STIN static __inline
#else
#define STIN static inline
#endif

#define T_COUNT 5
#define T_ROWS 22
#define T_COLS 10
#define PIECE_COUNT 7

enum Piece {
  kind_I = 0,
  kind_J = 1,
  kind_L = 2,
  kind_O = 3,
  kind_S = 4,
  kind_T = 5,
  kind_Z = 6
};

enum TickResult {
  move_down = 0,
  new_current = 1,
  game_over = 2
};

struct State{
  int score; 
  bool party;
  uint64_t x;
  int grid[T_ROWS][T_COLS];
  struct Tetromino *current;
  struct Tetromino *last;
};

struct Pos{
  int top;
  int left;
};

struct Tetromino{
  int shape[4][4];
  enum Piece kind;
  struct Pos pos;
  struct Tetromino *next;
};

struct Result{
  uint64_t n;
  uint64_t state;
};
// Xorshift64
STIN struct Result xorshift_next(uint64_t state){
  state ^= state >> 12;
  state ^= state << 25;
  state ^= state >> 27;
  struct Result r = {state * 0x2545F4914F6CDD1D, state};
  return r;
}

struct Tetromino *tetromino_create(enum Piece kind);
bool tetromino_delete(struct Tetromino *tetromino);

struct State *tetris_init(uint64_t seed);
bool tetris_exit(struct State *state);

enum TickResult tetris_tick(struct State *state);

enum TickResult tetris_drop(struct State *state);
void tetris_turn(struct State *state);
void tetris_move_r(struct State *state);
void tetris_move_l(struct State *state);

#endif
