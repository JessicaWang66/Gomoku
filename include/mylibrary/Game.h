//
// Created by yj17 on 4/21/2020.
//

#ifndef FINALPROJECT_GAME_H
#define FINALPROJECT_GAME_H

enum Stone { EMPTY, BLACK, WHITE };

class Game {
 public:
  // standard board size.
  static const int BOARD_SIZE = 19;
  // number of consecutive stone needed to win
  static const int WINNING_THRESHOLD = 5;
  // current winner
  Stone mWinner;
  // current player
  Stone mCurrentRole;
  // board
  Stone mChessStatus[BOARD_SIZE][BOARD_SIZE];

 public:
  Game();
  void Reset();
  /**
   * place a stone at given position
   * @param x row coordinate
   * @param y column coordinate
   * @return -1 if position is out of range or is occupied; or current winner.
   */
  Stone Play(int x, int y);
  /**
   * get the stone at given position
   * @param x row coordinate
   * @param y column coordinate
   * @return stone type at given position
   */
  Stone GetStatus(int x, int y);
  /**
   * get current player type.
   * @return current player type.
   */
  Stone GetRole();
  /**
   * check winner for the game. Since we check winner for every step, this
   * function only goes through the column, row, and two diagonals containing
   * latest position.
   * @param x  row coordinate
   * @param y  column coordinate
   * @return  whether current winner wins.
   */
  static bool IsWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y);

 private:
  /**
   * check whether current column contains winner.
   * @param x row coordinate
   * @param y column coordinate
   * @return whether has winner
   */
  static bool IsColumnWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y);
  /**
   * check whether current row contains winner.
   * @param x row coordinate
   * @param y column coordinate
   * @return whether has winner
   */
  static bool IsRowWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y);
  /**
   * check whether current diagonal contains winner.
   * @param x row coordinate
   * @param y column coordinate
   * @return whether has winner
   */
  static bool IsDiagonalWin(Stone board[BOARD_SIZE][BOARD_SIZE], int x, int y);
  /**
   * check whether current anti-diagonal contains winner.
   * @param x row coordinate
   * @param y column coordinate
   * @return whether has winner
   */
  static bool IsAntiDiagonalWIn(Stone board[BOARD_SIZE][BOARD_SIZE], int,
                                int y);
};
#endif  // FINALPROJECT_GAME_H
