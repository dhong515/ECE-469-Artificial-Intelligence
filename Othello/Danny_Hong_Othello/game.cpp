#ifndef _GAME_CPP_
#define _GAME_CPP_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <climits>
#include <unistd.h>
#include <vector>
#include <ctype.h>
#include "constraints.h"
#include "board.h"
#include "game.h"

using namespace std;

Game::Game(){} 

void Game::set(char mode){ 
  if(mode == 'A' || mode == 'a'){
    user[WHITE] = false;
    user[BLACK] = true;
  }
  else if(mode == 'B' || mode == 'b'){
    user[WHITE] = true;
    user[BLACK] = false;
  }
  else if(mode == 'C' || mode == 'c'){
    user[WHITE] = false;
    user[BLACK] = false;    
  }
  else{
    return;
  }

  char savedGameFile;

  cout << "WOULD YOU LIKE TO IMPORT A SAVED GAME FILE? (Y/N): ";
  cin >> savedGameFile;

  if(tolower(savedGameFile) == 'y'){ 
    string savedFileName;
    fstream boardFile;
    char startBoard[8][8], x;
    int player, i = 0, j = 0;

    cout << "PLEASE ENTER A NAME FOR THE SAVED GAME FILE: ";
    cin >> savedFileName;

    boardFile.open(savedFileName.c_str(), fstream::in);

    while(boardFile.fail()){
      cout << "INVALID GAME FILE NAME! PLEASE ENTER A VALID NAME: ";
      cin >> savedFileName;
      boardFile.open(savedFileName.c_str(), fstream::in);
    }

    while((x = boardFile.get()) != EOF){  
      if((x == '0') || (x == '1') || (x == '2')){
        startBoard[i][j++] = x-'0';
        if(j == 8){
          j = 0;
          if(++i == 8){
            break;
          }
        }
      }else if(x == '0'){
        startBoard[i][j++] = ' ';
      }
    }

    boardFile >> player;
    boardFile >> timeLimit;
    board = Board(startBoard, player); 

    boardFile.close();
  }
  else if(tolower(savedGameFile) == 'n'){
    string setTime;
    timeLimit = -1;
    while(timeLimit < 0){
      cout << "ENTER TIME LIMIT PER MOVE [5 - 60 SECONDS] ALOTTED FOR THE AI: ";
      cin >> setTime;
      if(isdigit(setTime.c_str()[0])){
        timeLimit = atoi(setTime.c_str());
      }
    }
    board = Board();  
  }
  else{
    cout << "PLEASE ENTER Y(YES) OR N(NO)" << endl;
  }
}

void Game::play(){  
  bool endGame = false;
  srand(time(NULL));

  cout << endl << "STARTING THE GAME. GOOD LUCK!" << endl;

  board.print();

  while(!endGame){
    if(user[board.currentPlayer]){
      endGame = userMove();
    }
    else{
      endGame = computerMove();
    }
  }
  board.print();
  board.gameOver();
}

bool Game::userMove(){ 
  int moveChoice = -1;
  char choice;

  vector<Board::Move> options = board.legalMoves(board.currentPlayer);

  if(options.size()){
    board.print(options);
    cout << "LEGAL MOVE OPTIONS:" << endl;

    for(int i = 0; i < options.size(); i++){
      cout << (char)(i + 65) << ": (" << (int)options[i].space.y << ", " << (int)options[i].space.x << ")" << endl;
    }

    while(!(moveChoice < options.size() && moveChoice >= 0)){
      cout << "PLEASE CHOOSE THE LETTER REPRESENTING YOUR MOVE CHOICE: ";
      cin >> choice;
      moveChoice = ((int)toupper(choice) - 65);
    }

    board.applyMove(options[moveChoice]);
    return board.changePlayer(false);
  }
  else{
    cout << "PASSING TURN: NO LEGAL MOVES PRESENT" << endl;
    return board.changePlayer(true);
  }
}

bool Game::computerMove(){  
  int depth, limit, value, moveChoice;
  limit = 64 - (board.scoreboard[BLACK] + board.scoreboard[WHITE]);
  Board::Move moveOption;

  timeStart = clock();
  playerMax = board.currentPlayer;

  vector<Board::Move> moveLegal = board.legalMoves(board.currentPlayer);

  if(moveLegal.size() == 0){
    cout << "PASSING TURN: NO LEGAL MOVES FOR THE COMPUTER" << endl;
    return board.changePlayer(true);
  }

  for(depth = 0; (((float)(clock() - timeStart))/CLOCKS_PER_SEC < timeLimit/2.0) && depth < limit; depth++){
    int random, alpha, beta;
    alpha = INT_MIN;
    beta = INT_MAX;
    timeout = false;
    random = 1;

    for(int i = 0; i < moveLegal.size(); i++){
      Board childBoard = board;
      childBoard.applyMove(moveLegal[i]);
      childBoard.changePlayer(false);
      value = alphaBeta(childBoard, depth, alpha, beta, false);

      if(timeout){
        break;
      }

      if(alpha < value){
        moveOption = moveLegal[i];
        moveChoice = i;
        alpha = value;
      }
      else if(alpha == value){
        if(((rand() % random++) - 1) == 0){
          moveChoice = i;
          moveOption = moveLegal[i];
        }
      }
    }
  }

  cout << "THE MAX DEPTH SEARCHED BY AI IS: " << depth << " [TIME ELAPSED = " << ((float)(clock()-timeStart))/CLOCKS_PER_SEC << " seconds]" << endl;
  cout << "LEGAL MOVE OPTIONS FOR AI: " << endl;
  for(int i = 0; i < moveLegal.size(); i++){
    cout << (char)(i + 65) << ": (" << (int)moveLegal[i].space.y << ", " << (int)moveLegal[i].space.x << ")" << endl;
  }
  board.print(vector<Board::Move>(1, moveOption), true);
  cout << "CHOSEN MOVE BY THE AI IS: " << (char)(moveChoice + 65) << endl;
  board.applyMove(moveOption);

  return board.changePlayer(false);
}

int Game::alphaBeta(Board board, int depth, int alpha, int beta, bool playerMax){ 
  int a, b;
  a = alpha;
  b = beta;

  if((((float)(clock()-timeStart))/CLOCKS_PER_SEC) > (CUTOFFTIME * timeLimit)){
    timeout = true;
    return heuristic(board);
  }
  else if(depth == 0){
    return heuristic(board);
  }
  else{
    depth = depth - 1;
  }

  vector<Board::Move> moveOption = board.legalMoves(board.currentPlayer);

  if(moveOption.size() == 0){
    if(board.checkGameEnd(true)){
      Board childBoard = board;
      childBoard.changePlayer(false);
      return heuristic(childBoard);
    }else{
      Board childBoard = board;
      childBoard.changePlayer(true);
      return alphaBeta(childBoard, depth, alpha, beta, !playerMax);
    }
  }

  if(playerMax){  
    int v = INT_MIN;

    for(int i = 0; i < moveOption.size(); i++){
      Board childBoard = board;
      childBoard.applyMove(moveOption[i]);
      childBoard.changePlayer(false);

      int value = alphaBeta(childBoard, depth, a, b, false);
      v = MAX(v, value);

      if(v >= beta){
        return v;
      }
      int a = MAX(a,v);
    }

    return v;
  }
  else{ 
    int v = INT_MAX;

    for(int i = 0; i < moveOption.size(); i++){
      Board childBoard = board;
      childBoard.applyMove(moveOption[i]);
      childBoard.changePlayer(false);

      int value = alphaBeta(childBoard, depth, a, b, true);
      v = MIN(v, value);

      if(v <= a){
        return v;
      }
      b = MIN(b, v);
    }

    return v;
  }
}

int Game::heuristic(Board board){ 
  int opponent, playerCorners = 0, opponentCorners = 0;
  double pieceCount, frontier, corner, mobility, edge, possibleCorners;

  if(playerMax == BLACK){
    opponent = WHITE;
  }
  else{
    opponent = BLACK;
  }

  pieceCount = ((100.0*board.scoreboard[playerMax])/(board.scoreboard[playerMax] + board.scoreboard[opponent]));

  if(board.board[0][0] == playerMax){ 
    playerCorners = playerCorners + 1;
  }
  else if(board.board[0][0] == opponent){
    opponentCorners = opponentCorners + 1;
  }
  if(board.board[0][7] == playerMax){
    playerCorners = playerCorners + 1;
  }
  else if(board.board[0][7] == opponent){
    opponentCorners = opponentCorners + 1;
  }
  if(board.board[7][0] == playerMax){
    playerCorners = playerCorners + 1;
  }
  else if(board.board[7][0] == opponent){
    opponentCorners = opponentCorners + 1;
  }
  if(board.board[7][7] == playerMax){
    playerCorners = playerCorners + 1;
  }
  else if(board.board[7][7] == opponent){
    opponentCorners = opponentCorners + 1;
  }

  corner = 25.0*(playerCorners - opponentCorners);

  int playerEdges = 0, opponentEdges = 0, playerFrontier = 0, opponentFrontier = 0;

  for(int i = 0; i < 8; i++){ 
    for(int j = 0; j < 8; j++){
      if(i == 0 || i == 7 || j == 0 || j == 7){
        if(board.board[i][j] == playerMax){
          playerEdges = playerEdges + 1;
        }
        else if(board.board[i][j] == opponent){
          opponentEdges = opponentEdges + 1;
        }
      }else if(board.board[i][j] != ' '){
        if(board.checkOnFrontier(j, i)){  
          if(board.board[i][j] == playerMax){
            playerFrontier = playerFrontier + 1;
          }
          else if(board.board[i][j] == opponent){
            opponentFrontier = opponentFrontier + 1;
          }
        }
      }
    }
  }

  edge = (100.0*playerEdges)/(playerEdges + opponentEdges);
  frontier = -100*(playerFrontier - opponentFrontier);

  vector<Board::Move> playerLegal = board.legalMoves(playerMax);
  vector<Board::Move> opponentLegal = board.legalMoves(opponent);
  mobility = (100.0*playerLegal.size())/(playerLegal.size() + opponentLegal.size());

  opponentCorners = 0;
  for(int i = 0; i < opponentLegal.size(); i++){
    if(opponentLegal[i].space.x == 0 && opponentLegal[i].space.y == 0){
      opponentCorners = opponentCorners + 1;
    }
    else if(opponentLegal[i].space.x == 7 && opponentLegal[i].space.y == 0){
      opponentCorners = opponentCorners + 1;
    }
    else if(opponentLegal[i].space.x == 0 && opponentLegal[i].space.y == 7){
      opponentCorners = opponentCorners + 1;
    }
    else if(opponentLegal[i].space.x == 7 && opponentLegal[i].space.y == 7){
      opponentCorners = opponentCorners + 1;
    }
  }

  possibleCorners = -25.0*opponentCorners;

  int pieceWeight = 2*(board.scoreboard[playerMax] + board.scoreboard[opponent]);
  int possibleCornerWeight = 3000;
  int frontierWeight = 300;
  int edgeWeight = 70;
  int cornerWeight = 6000;
  int mobilityWeight = 10*(100-(board.scoreboard[playerMax] + board.scoreboard[opponent]));

  double netTotal = (pieceWeight * pieceCount) + (edgeWeight * edge) + (frontierWeight * frontier) + (cornerWeight * corner) + (possibleCornerWeight * possibleCorners) + (mobilityWeight * mobility);

  return netTotal;
}

#endif