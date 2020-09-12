//
// Created by yj17 on 5/4/2020.
//

#ifndef FINALPROJECT_SIMPLEAUTOPLAYER_H
#define FINALPROJECT_SIMPLEAUTOPLAYER_H
#include "Game.h"
enum Direction { ROW, COlUMN, DIAGONAL, ANTI_DIAGONAL };
/**
 * simple auto player strategy evaluate all possible position
 * within one move
 */
class SimpleAutoPlayer {
 public:
  /**
   * make best move within 1 move.
   *
   * @param board board status
   * @param player current player
   * @param best_x best_x reference
   * @param best_y best y reference
   * @return numerical value of the move
   */
  static int SimpleStrategy(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                            Stone player, int& best_x, int& best_y);

 private:
  /**
   * this function is partially cited from
   * https://www.shangmayuan.com/a/8bca21de88124d49a6cd9ec2.html
   *
   * @param board the board status
   * @param row row index
   * @param column column index
   * @param player current player
   * @return the numerical value of the move
   */
  static int Evaluate(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], int row,
                      int column, Stone player);
  /**
   * this function is partially cited from
   * https://www.shangmayuan.com/a/8bca21de88124d49a6cd9ec2.html
   *
   * @param board board status
   * @param row row index
   * @param column column index
   * @param player current player
   * @param direction the direction; horizontal, vertical, diagonal, or
   * anti-digonal
   * @return the numerical value of the move on this direction
   */
  static int EvaluateDirection(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                               int row, int column, Stone player,
                               Direction direction);
};
#endif  // FINALPROJECT_SIMPLEAUTOPLAYER_H
