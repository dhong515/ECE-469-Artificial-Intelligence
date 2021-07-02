#ifndef _GAME_H
#define _GAME_H

#include <time.h>
#include "constraints.h"
#include "board.h"

using namespace std;

class Game{
public:
  Game(); 
  void set(char mode);
  void play();

private:
  Board board;
  clock_t timeStart;
  bool timeout;
  bool user[3];
  int timeLimit;
  int playerMax;  
  bool userMove();
  bool computerMove();
  int alphaBeta(Board board, int depth, int alpha, int beta, bool playerMax);
  int heuristic(Board board);
};

#endif 

