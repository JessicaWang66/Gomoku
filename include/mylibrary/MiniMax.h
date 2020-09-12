//
// Created by yj17 on 4/22/2020.
//

#ifndef FINALPROJECT_MINIMAX_H
#define FINALPROJECT_MINIMAX_H

#include <Windows.h>

#include <cmath>
#include <tuple>
// we only care about 7 consecutive stones on the board
// each stone convert to 2 bit number, 00 , 01, or 11
// therefore, max size would be 2^(7 * 2) = 0x3fff
static const int BIT_DATA_LENGTH = 14;
static const int BIT_DATA_SIZE = 0x3fff;  // NOLINT
// search for empty grid within 2 unit of the occupied grid.
// increase this value could slightly increase win rate
// but exponentially decrease efficiency
static const int SEARCH_RANGE = 2;
// default search depth. Increase depth will significantly increase win rate
// while factorial increase time needed to finish computation
static const int SEARCH_DEPTH = 3;
// multiple-thread number, default set to be 4. No harm to be higher.
static const int THREAD_NUM = 4;

/**
 * all gomoku stone patterns that can contribute to winning.
 */
enum PatternType {
  NONE,
  CONSECUTIVE_FIVE,
  OPEN_FOUR,
  MAKE_CONSECUTIVE_FOUR,
  OPEN_THREE,
  HALF_OPEN_THREE,
  OPEN_TWO,
  HALF_OPEN_TWO,
  CLOSE_FOUR,
  CLOSE_THREE,
  CLOSE_TWO
};
/**
 * pattern struct used to store char pattern
 * and corresponding score and type enum.
 */
struct Pattern {
  char string_pattern[BIT_DATA_LENGTH / 2 + 1];
  int score;
  int type;
};
/**
 * score cache used to store highest score in each
 * row, column, diagonal, anti-diagonal score and pattern type
 */
struct ScoreCache {
  int horizontal_score[Game::BOARD_SIZE];
  int horizontal_type[Game::BOARD_SIZE];
  int vertical_score[Game::BOARD_SIZE];
  int vertical_type[Game::BOARD_SIZE];
  int diagonal_score[2 * Game::BOARD_SIZE - 1];
  int diagonal_type[2 * Game::BOARD_SIZE - 1];
  int antiDiagonal_score[2 * Game::BOARD_SIZE - 1];
  int antiDiagonal_type[2 * Game::BOARD_SIZE - 1];
};
/**
 * doubly linked list used to store candidate position
 * (empty grid that has neighbor within 2 grid distance)
 * sorted by point value from highest to lowest.
 */
class CandidatePosition {
 public:
  CandidatePosition() {
    pNext = nullptr;
    pPrev = nullptr;
  }
  CandidatePosition(int x, int y, int value) {
    row_index = x;
    column_index = y;
    grid_value = value;
    pPrev = nullptr;
    pNext = nullptr;
  }
  int row_index{}, column_index{}, grid_value{};
  CandidatePosition *pNext, *pPrev;
};
/**
 * alpha-beta pruning to find best move on given board
 *
 * search range, depth, and multiple-thread number can be edit
 * on the top of this file. (adjust in accordance with computer
 * computing ability).
 */
class AlphaBetaAlgorithm {
 public:
  AlphaBetaAlgorithm();
  /**
   * alpha-beta prunning find best position to move
   * @param board board status
   * @param player current player
   * @param x row index reference. Will be updated to best row index
   * @param y column index reference. Will be updated to best column index
   * @return 1 for normal situation and 0 for no move available
   */
  int AlphaBetaGo(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player,
                  int& x, int& y);
  /**
   * alpha-beta pruning find best position to move with multiple thread
   * @param board board status
   * @param player current player
   * @param x row index reference. Will be updated to best row index
   * @param y column index reference. Will be updated to best column index
   * @return 1 for normal situation and 0 for no move available
   */
  int AlphaBetaGoMT(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                    Stone player, int& x, int& y);

 private:
  /**
   * initialize board state to score table.
   * convert each board state to binary mask number and lable its type.
   */
  void InitScoreTable();
  /**
   * using minimax algorithm to perform alternating simulation for
   * both side of the player to find the best grid with in n steps.
   *
   * n is the search depth (as defined in Minimax header file). Increase
   * the search depth will significantly increase win rate but also
   * increase calculation in factorial manner.
   *
   * @param board board status
   * @param depth search depth (default is 3)
   * @param maxPlayer max player
   * @param player current player
   * @param alpha alpha value
   * @param beta beta value
   * @return the score
   */
  int MinMax(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], int depth,
             Stone maxPlayer, Stone player, int alpha, int beta);
  /**
   * calculate the numeric value of the board
   *
   * this function is the most important function to increase win rate.
   * the main idea is to create a hash table for the board and store
   * value in ScoreCache strcture.
   *
   * All its helper functions have run time below O(n^2) with some
   * helper functions have run time about O(nlogn).
   *
   * @param board board status
   * @param maxPlayer maximum player (the auto player)
   * @return numerically valuation of the board
   */
  int EvaluateMinMax(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                     Stone maxPlayer);
  /**
   * check if the position is valid. A valid position means there's at least
   * one occupied grid within the range of target grid.
   *
   * the range variable is defined Minimax header file. Change its value will
   * result in slightly higher win rate with much lower efficiency.
   *
   * @param board board status
   * @param x x coordinate
   * @param y y coordinate
   * @return whether the position is valid.
   */
  static bool IsValidPosition(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                              int x, int y);
  /**
   * perform search for candidate positions for given player.
   * use doubly linked list to sort them by point value
   * this functions is to increase pruning efficiency
   *
   * @param chess board state
   * @param player current player
   * @return the root of doubly linked list
   */
  CandidatePosition* SearchCandidatePosition(
      Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player);
  /**
   * score the whole chess baord and store score for each point in each
   * direction in ScoreCache structure
   *
   * this function is used to store the board the passed to alpha-beta-go
   * since we don't need to perform duplicate calculation in later simulation
   *
   * @param board board status
   * @param player target player
   * @param pCache SchoreCache instance
   */
  void ScoreChessToCache(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                         Stone player, ScoreCache* pCache);
  /**
   * calculate point value at given position and store it ScoreCache structure
   * helper function for evaluate minimax
   *
   * @param board board status
   * @param player player type
   * @param x target x
   * @param y target y
   * @param pCache pCache pointer
   */
  void ScoreChessPointToCache(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                              Stone player, int x, int y, ScoreCache* pCache);
  /**
   * retrieve final score for the board based on ScoreCache table
   * @param pCache ScoreCache pointer
   * @return final score for the board
   */
  static int ScoreChess(ScoreCache* pCache);
  /**
   * get the score of the certain direction containing certain point
   *
   * this function is used to perform candidate position sort
   * in order to increase efficiency of alpha-beta pruning
   *
   * this function is helper function for ScorePoint.
   * @param board board status
   * @param player target player
   * @param dir direction index. row(0), column(1), diagonal(2), anti(3)
   * @param x target x coordinate
   * @param y target y coordinate
   * @param type best type reference
   * @return best score in this direction containing this point.
   */
  int ScorePointDir(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                    Stone player, int dir, int x, int y, int& type);
  /**
   * get the score of target point.
   * this function is used to perform candidate position sort
   * in order to increase efficiency of alpha-beta pruning
   *
   * @param board board status
   * @param player target player
   * @param x target x coordinate
   * @param y target y coordinate
   * @return the score of the point
   */
  int ScorePoint(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player,
                 int x, int y);
  /**
   * get winner of the board
   * @param board board status
   * @return winner stone type (empty for draw case)
   */
  static int GetWinner(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE]);
  /**
   * find mask in address snippet
   * @param addr address that need to be checked
   * @param addrBitCount address bit count
   * @param mask target mask
   * @param maskBitCount mask bit count
   * @return whether address contains mask snippet
   */
  static bool IsAddrContainsMask(int addr, int addrBitCount, int mask,
                                 int maskBitCount);

 private:
  // store score of every combination of 7 consecutive stones
  int score_table[BIT_DATA_SIZE];
  // store pattern type of every combination of 7 consecutive stones
  int score_type_table[BIT_DATA_SIZE];
  // backup board used for simulation
  int board_backup[Game::BOARD_SIZE][Game::BOARD_SIZE];
  // black side score cache
  ScoreCache black_score_cache;
  // white side score cache
  ScoreCache white_score_cache;
  // hard-coding pattern (seems like this can only be hard coding)
  Pattern pattern[28] = {{"11111", 1000000, CONSECUTIVE_FIVE},
                         {"011110", 11000, OPEN_FOUR},
                         {"0011112", 220, MAKE_CONSECUTIVE_FOUR},
                         {"0101112", 220, MAKE_CONSECUTIVE_FOUR},
                         {"0110112", 220, MAKE_CONSECUTIVE_FOUR},
                         {"0111012", 220, MAKE_CONSECUTIVE_FOUR},
                         {"0110110", 220, MAKE_CONSECUTIVE_FOUR},
                         {"0101110", 220, MAKE_CONSECUTIVE_FOUR},
                         {"001110", 210, OPEN_THREE},
                         {"010110", 210, OPEN_THREE},
                         {"001112", 60, HALF_OPEN_THREE},
                         {"010112", 60, HALF_OPEN_THREE},
                         {"011012", 60, HALF_OPEN_THREE},
                         {"10011", 60, HALF_OPEN_THREE},
                         {"10101", 60, HALF_OPEN_THREE},
                         {"2011102", 60, HALF_OPEN_THREE},
                         {"00110", 15, OPEN_TWO},
                         {"01010", 15, OPEN_TWO},
                         {"010010", 15, OPEN_TWO},
                         {"000112", 13, HALF_OPEN_TWO},
                         {"001012", 13, HALF_OPEN_TWO},
                         {"010012", 13, HALF_OPEN_TWO},
                         {"10001", 13, HALF_OPEN_TWO},
                         {"2010102", 13, HALF_OPEN_TWO},
                         {"2011002", 13, HALF_OPEN_TWO},
                         {"211112", 0, CLOSE_FOUR},
                         {"21112", 0, CLOSE_THREE},
                         {"2112", 0, CLOSE_TWO}};

  /**
   * execute minimax algorithm to find numeric value of the point.
   * retrieve board and point info from program
   *
   * @param pParam program pointer
   * @return unassigned int representing status
   */
  static DWORD WINAPI MinMaxThread(void* pParam);
};
/**
 * struct used to pass all arguments to multiple thread program.
 * Since it only accepts one argument, we wrap it up in a
 * single struct.
 */
struct MinMaxThreadParam {
  AlphaBetaAlgorithm* pAlgorithm;
  Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE];
  int x;
  int y;
  int depth;
  Stone maxPlayer;
  Stone player;
  int bestValue;
};

#endif  // FINALPROJECT_MINIMAX_H
