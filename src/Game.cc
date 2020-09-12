//
// Created by yj17 on 4/21/2020.
//
#include "mylibrary/Game.h"

#include <cstring>
Game::Game() { Reset(); }
void Game::Reset() {
  // reset all grid to empty
  std::memset(mChessStatus, Stone::EMPTY,
              sizeof(int) * BOARD_SIZE * BOARD_SIZE);
  // rest current role to black player
  mCurrentRole = Stone::BLACK;
  // reset winner to empty
  mWinner = Stone::EMPTY;
}
Stone Game::GetRole() { return mCurrentRole; }
Stone Game::Play(int row_index, int column_index) {
  // if given position is out of range, return -1.
  if (row_index < 0 || row_index >= BOARD_SIZE || column_index < 0 ||
      column_index >= BOARD_SIZE) {
    return static_cast<Stone>(-1);
  } else if (mChessStatus[row_index][column_index] != 0) {
    // if given position is occupied, return -1
    return static_cast<Stone>(-1);
  }
  // place a stone at given position
  mChessStatus[row_index][column_index] = mCurrentRole;
  // if has winner, update winner, and set player to empty
  // otherwise switch current game player
  if (IsWin(mChessStatus, row_index, column_index)) {
    mWinner = mCurrentRole;
    mCurrentRole = Stone::EMPTY;
  } else if (mCurrentRole == 1) {
    mCurrentRole = Stone::WHITE;
  } else {
    mCurrentRole = Stone::BLACK;
  }
  return mWinner;
}
Stone Game::GetStatus(int row_index, int column_index) {
  // if given position is out of range, return -1.
  if (row_index < 0 || row_index >= BOARD_SIZE || column_index < 0 ||
      column_index >= BOARD_SIZE) {
    return static_cast<Stone>(-1);
  }
  return mChessStatus[row_index][column_index];
}
bool Game::IsWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
  return IsColumnWin(board, x, y) || IsRowWin(board, x, y) ||
         IsDiagonalWin(board, x, y) || IsAntiDiagonalWIn(board, x, y);
}
bool Game::IsColumnWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
  int count = 0;
  // go downwards
  int new_y = y;
  while (--new_y >= 0 && board[x][new_y] == board[x][y]) count++;
  // go upwards
  new_y = y;
  while (++new_y < BOARD_SIZE && board[x][new_y] == board[x][y]) count++;
  // return whether win based on consecutive count.
  return count >= WINNING_THRESHOLD - 1;
}
bool Game::IsRowWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
  int count = 0;
  int new_x = x;
  // go right
  while (++new_x < BOARD_SIZE && board[new_x][y] == board[x][y]) count++;
  // go left
  new_x = x;
  while (--new_x >= 0 && board[new_x][y] == board[x][y]) count++;
  // return whether win based on consecutive count.
  return count >= WINNING_THRESHOLD - 1;
}
bool Game::IsDiagonalWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
  int new_x = x;
  int new_y = y;
  int count = 0;
  // go bottom left
  while (--new_x >= 0 && --new_y >= 0 && board[new_x][new_y] == board[x][y]) {
    count++;
  }
  // go upper right
  new_x = x;
  new_y = y;
  while (++new_x < BOARD_SIZE && ++new_y < BOARD_SIZE &&
         board[new_x][new_y] == board[x][y]) {
    count++;
  }
  // return whether win based on consecutive count.
  return count >= WINNING_THRESHOLD - 1;
}
bool Game::IsAntiDiagonalWIn(Stone board[BOARD_SIZE][BOARD_SIZE], int x,
                             int y) {
  int count = 0;
  // go bottom right
  int new_x = x;
  int new_y = y;
  while (++new_x < BOARD_SIZE && --new_y >= 0 &&
         board[new_x][new_y] == board[x][y]) {
    count++;
  }
  // go upper left
  new_x = x;
  new_y = y;
  while (--new_x >= 0 && ++new_y < BOARD_SIZE &&
         board[new_x][new_y] == board[x][y]) {
    count++;
  }
  // return whether win based on consecutive count.
  return count >= WINNING_THRESHOLD - 1;
}
