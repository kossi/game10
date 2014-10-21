#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "game.h"

int main(int argc, char *argv[]){
  long seed;
  if(argc > 1)
    seed = atol(argv[1]);
  else
    seed = time(NULL);

  int score = start(seed);
  printf("Your Score: %i, The Seed: %lu\n", score, seed);
}

