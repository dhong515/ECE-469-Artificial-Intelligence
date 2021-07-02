#ifndef _BOARD_H
#define _BOARD_H

#include <vector>
#include "constraints.h"

using namespace std;

class Board{
public:

  class Space{
  public:
    char x, y;
    Space();  
    Space(char x, char y);
  };

  class Move{
  public:
    bool validMove;
    Board::Space space;
    vector<Space> flips;
    Move();
    Move(char x, char y);
  };

  Board();
  Board(char state[8][8], int currentPlayer);
  Board(Board &board);
  vector<Board::Move> legalMoves(int player);
  char board[8][8];
  bool passedPlayer;
  int currentPlayer;
  int scoreboard[3];
  bool checkOnFrontier(int x, int y);
  void applyMove(Board::Move chosenMove);
  void print(vector<Board::Move> moves = vector<Board::Move>(), bool computerMove = false);
  bool checkGameEnd(bool passedCurrentPlayer);
  bool changePlayer(bool passedCurrentPlayer);
  void gameOver();

private:
  bool traverse(int mode, int direction, char &x, char &y);
  bool checkOnBoard(const char x, const char y);
};

#endif 