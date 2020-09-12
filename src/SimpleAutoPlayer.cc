//
// Created by yj17 on 5/4/2020.
//

#include "mylibrary/SimpleAutoPlayer.h"

#include <algorithm>
int SimpleAutoPlayer::Evaluate(Stone (*board)[Game::BOARD_SIZE], int row,
                               int column, Stone player) {
  int value = 0;
  // evaluate for the row
  value += EvaluateDirection(board, row, column, player, ROW);
  // evaluate for the column
  value += EvaluateDirection(board, row, column, player, COlUMN);
  // evaluate for the diagonal
  value += EvaluateDirection(board, row, column, player, DIAGONAL);
  // evaluate for the anti-diagonal
  value += EvaluateDirection(board, row, column, player, ANTI_DIAGONAL);
  // return the sum
  return value;
}

int SimpleAutoPlayer::EvaluateDirection(Stone (*board)[Game::BOARD_SIZE],
                                        int row, int column, Stone player,
                                        Direction direction) {
  // score table, seems like can only be hard coding
  // numbers can be changed, this is not the best solution
  // but not yet sure how to improve those values
  int ScoreTable[10] = {7, 35, 800, 15000, 800000, 15, 400, 1800, 100000, 0};
  int value = 0;
  // iterate through the horizontal index
  for (int i = -Game::WINNING_THRESHOLD + 1; i <= 0; i++) {
    int black_count = 0;
    int white_count = 0;
    // iterate through the vertial index
    for (int j = 0; j < Game::WINNING_THRESHOLD; j++) {
      int row_index = 0;
      int column_index = 0;
      switch (direction) {
        case ROW:
          row_index = row + i + j;
          column_index = column;
          break;
        case COlUMN:
          row_index = row;
          column_index = column + i + j;
          break;
        case DIAGONAL:
          row_index = row + i + j;
          column_index = column + i + j;
          break;
        case ANTI_DIAGONAL:
          row_index = row - i - j;
          column_index = column + i + j;
          break;
      }
      // if out of bound, reset
      if (row_index < 0 || row_index >= Game::BOARD_SIZE || column_index < 0 ||
          column_index >= Game::BOARD_SIZE) {
        black_count = 0;
        white_count = 0;
        break;
      }
      // update count
      if (board[row_index][column_index] != Stone::EMPTY) {
        if (board[row_index][column_index] == player)
          black_count++;
        else
          white_count++;
      }
    }
    if (black_count > 0 && white_count > 0)
      value += ScoreTable[2 * Game::WINNING_THRESHOLD - 1];
    else if (black_count == 0 && white_count == 0)
      value += ScoreTable[0];
    else if (black_count > 0)
      value += ScoreTable[black_count];
    else if (white_count > 0)
      value += ScoreTable[white_count + Game::WINNING_THRESHOLD - 1];
  }
  return value;
}
int SimpleAutoPlayer::SimpleStrategy(Stone (*board)[Game::BOARD_SIZE],
                                     Stone player, int& best_x, int& best_y) {
  // record current best x and y, and corresponding value
  int max_x = (int)(Game::BOARD_SIZE / 2);
  int max_y = (int)(Game::BOARD_SIZE / 2);
  int max_value = 0;
  // check if it's the first stone in the game
  bool is_first = true;
  for (int i = 0; i < Game::BOARD_SIZE; i++) {
    for (int j = 0; j < Game::BOARD_SIZE; j++)
      if (board[i][j] != Stone::EMPTY) {
        is_first = false;
        break;
      }
    if (!is_first) break;
  }
  // if yes, place the stone in the middle of the board
  if (is_first) {
    best_x = max_x;
    best_y = max_y;
    return 1;
  }
  // iterate through every empty grid
  for (int x = 0; x < Game::BOARD_SIZE; x++) {
    for (int y = 0; y < Game::BOARD_SIZE; y++) {
      if (board[x][y] == Stone::EMPTY) {
        // evaluate numeric value for this grid
        int value = Evaluate(board, x, y, player);
        // if is greater or equal to max value (with 30% random disturbance)
        if (value > max_value || (value == max_value && rand() % 100 < 30)) {
          max_value = value;
          max_x = x;
          max_y = y;
        }
      }
    }
  }
  // update best row and column coordinate
  if (max_value > 0) {
    best_x = max_x;
    best_y = max_y;
  }
  // return best value
  return max_value;
}
