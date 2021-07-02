#ifndef _BOARD_CPP_
#define _BOARD_CPP_

#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "board.h"
#include "constraints.h"

using namespace std;

int directions[2] = {-1, 1};
int modes[4] = {1, 2, 3, 4};

Board::Space::Space(){} 

Board::Space::Space(char x, char y){ 
  this->x = x;
  this->y = y;
}

Board::Move::Move(){} 

Board::Move::Move(char x, char y){  
  this->validMove = false;
  this->space.x = x;
  this->space.y = y;
}

Board::Board(){ 
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if((i == 4 && j == 3) || (i == 3 && j == 4)){
        board[i][j] = WHITE;
        }
      else if((i == 4 && j == 4) || (i == 3 && j == 3)){
        board[i][j] = BLACK;
        }
      else{
        board[i][j] = ' ';
      }            
    }
  }
  passedPlayer = false;
  currentPlayer = BLACK;
  scoreboard[BLACK] = 2;
  scoreboard[WHITE] = 2; 
}

Board::Board(Board &board){ 
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      this->board[i][j] = board.board[i][j];
    }
  }
  for(int i = 0; i < 2; i++){
    this->scoreboard[i] = board.scoreboard[i];
  }

  this->currentPlayer = board.currentPlayer;
  this->passedPlayer = board.passedPlayer;
}

Board::Board(char state[8][8], int currentPlayer){ 
  scoreboard[BLACK] = 0;
  scoreboard[WHITE] = 0;

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      board[i][j] = state[i][j];
      if(state[i][j] == BLACK){
        scoreboard[BLACK]= scoreboard[BLACK] + 1;
      }
      else if(state[i][j] == WHITE){
        scoreboard[WHITE] = scoreboard[WHITE] + 1;
      }
    }
  }

  passedPlayer = false;
  this->currentPlayer = currentPlayer;
}

bool Board::checkOnFrontier(int x, int y){ 
  if(board[y][x] == ' '){
    return false;
  }
  for(int i = 0; i < 4; i++){
    int mode = modes[i];
    for(int j = 0; j < 2; j++){
      int direction = directions[j];
      char X = x, Y = y;
      traverse(mode, direction, X, Y);
      if(checkOnBoard(X, Y)){
        if(board[Y][X] != ' '){
          return true;
        }
      }
    }
  }
  return false;
}

void Board::print(vector<Board::Move> moves, bool computerMove){ 
  cout << endl << "    0   1   2   3   4   5   6   7" << endl;
  cout << "  ---------------------------------" << endl;
  for(int i = 0; i < 8; i++){
    cout << i << " |";
    for(int j = 0; j < 8; j++){
      bool options = false;
      for(int k = 0; k < moves.size(); k++){
        if(moves[k].space.y == i && moves[k].space.x == j){
          if(!computerMove){
            cout << GREEN << setw(2) << (char)(k + 65) << RESET << setw(2) << "|";
          }
          else{
            cout << YELLOW << setw(2) << "AI" << RESET << setw(2) << "|";
          }
          options = true;
        }
      }
      if(!options){
        if(board[i][j] == WHITE){
          cout << RED;
          cout << setw(2) << (int)board[i][j] << RESET << setw(2) << "|";
        }
        else if(board[i][j] == BLACK){
          cout << BLUE;
          cout << setw(2) << (int)board[i][j] << RESET << setw(2) << "|";
        }
        else{
          cout << setw(2) << (char)board[i][j] << RESET << setw(2) << "|";
        }
      }
    }
    cout << endl;
    cout << "  ---------------------------------" << endl;
  }
  cout << endl;
}

bool Board::checkGameEnd(bool passedCurrentPlayer){  
  if((scoreboard[BLACK] + scoreboard[WHITE] == 64) || (passedPlayer && passedCurrentPlayer)){
    return true;
  }
  else{
    return false;
  }
}

bool Board::changePlayer(bool passedCurrentPlayer){  
  if(checkGameEnd(passedCurrentPlayer)){
    return true;
  }
  if(currentPlayer == BLACK){
    currentPlayer = WHITE;
  }
  else{
    currentPlayer = BLACK;
  }
  passedPlayer = currentPlayer;
  return false;
}

void Board::applyMove(Board::Move chosenMove){  
  board[chosenMove.space.y][chosenMove.space.x] = currentPlayer;

  if(currentPlayer == WHITE){
    scoreboard[BLACK] = scoreboard[BLACK] + 1;
  }
  else{
    scoreboard[BLACK] = scoreboard[BLACK] + 1;
  }

  for(int i = 0; i < chosenMove.flips.size(); i++){
    board[chosenMove.flips[i].y][chosenMove.flips[i].x] = currentPlayer;
    if(currentPlayer == WHITE){
      scoreboard[WHITE] = scoreboard[WHITE] + 1;
      scoreboard[BLACK] = scoreboard[BLACK] - 1;
    }
    else{
      scoreboard[WHITE] = scoreboard[WHITE] - 1;
      scoreboard[BLACK] = scoreboard[BLACK] + 1;
    }
  }
}

void Board::gameOver(){  
  cout << "GAME OVER! " << endl;
  cout << "GOOD GAME!  FINAL SCORE - BLACK: " << scoreboard[BLACK] << " | WHITE: " << scoreboard[WHITE] << endl;
  if(scoreboard[BLACK] < scoreboard[WHITE]){
    cout << "WHITE HAS WON THE GAME!" << endl;
  }
  else if(scoreboard[BLACK] > scoreboard[WHITE]){
    cout << "BLACK HAS WON THE GAME!" << endl;
  }
  else{
    cout << "TIE GAME!" << endl;
  }
}

bool Board::checkOnBoard(const char x, const char y){
  return ((x >= 0) && (x < 8) && (y >= 0) && (y < 8));
}

bool Board::traverse(int mode, int direction, char &x, char &y){ 
  if((direction != -1) && (direction != 1)){
    cout << "INVALID TRAVERSING INCREMENT! " << direction << endl;
    return false;
  }
  if(mode == 1){
    y = y + direction;
  }
  else if(mode == 2){
    x = x + direction;
  }
  else if(mode == 3){
    y = y + direction;
    x = x + direction;
  }
  else if(mode == 4){
    y = y + direction;
    x = x - direction;
  }
  else{
    cout << "ERROR! TRAVERSING IN AN INVALID DIRECTION " << mode << endl;
    return false;
  }
  return true;
}

vector<Board::Move> Board::legalMoves(int player){ 
  vector<Board::Move> moveList;

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if(board[i][j] != ' '){ 
        continue; 
      }

      Board::Move free = Board::Move(j, i);

      for(int k = 0; k < 4; k++){
        int mode = modes[k];

        for(int l = 0; l < 2; l++){
          int direction = directions[l];
          char x = free.space.x, y = free.space.y;
      
          vector<Board::Space> follow; 
          traverse(mode, direction, x, y);

          if((board[y][x] == ' ') || (board[y][x] == player)){
            continue; 
          }

          for(x, y; checkOnBoard(x, y); traverse(mode, direction, x, y)){
            if(board[y][x] == player){
              free.validMove = true;
              free.flips.insert(free.flips.end(), follow.begin(), follow.end());
              break;
            }
            else if(board[y][x] == ' '){
              break;
            }
            else{
              follow.push_back(Board::Space(x, y));
            }
          }
        }
      }
      if(free.validMove){
        moveList.push_back(free);
      }
    }
  }
  return moveList;
}

#endif