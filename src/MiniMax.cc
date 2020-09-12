#include <mylibrary/Game.h>
#include <mylibrary/MiniMax.h>

#include <algorithm>
using std::max;
using std::min;

AlphaBetaAlgorithm::AlphaBetaAlgorithm() { InitScoreTable(); }

void AlphaBetaAlgorithm::InitScoreTable() {
  // reset score table and score table type array
  memset(score_table, 0, sizeof(score_table));
  memset(score_type_table, 0, sizeof(score_type_table));
  // iterator through every gomoku type
  for (auto& i : pattern) {
    int mask1 = 0;  // forward mask
    int mask2 = 0;  // backward mask
    int length = strlen(i.string_pattern);
    for (int j = 0; j < length; j++) {
      mask1 <<= 2;  // NOLINT
      mask2 <<= 2;  // NOLINT
      // update forward mask
      switch (i.string_pattern[j]) {
        case '1':
          mask1 |= 1;  // NOLINT
          break;
        case '2':
          mask1 |= 2;  // NOLINT
          break;
      }
      // update backward mask
      switch (i.string_pattern[length - j - 1]) {
        case '1':
          mask2 |= 1;  // NOLINT
          break;
        case '2':
          mask2 |= 2;  // NOLINT
          break;
      }
    }
    // check if address contains target mask snippet
    // if yes, record current address to score convertion table
    for (int addr = 0; addr < BIT_DATA_SIZE; addr++) {
      if (score_table[addr] == 0) {
        if (IsAddrContainsMask(addr, BIT_DATA_LENGTH, mask1, length * 2) ||
            IsAddrContainsMask(addr, BIT_DATA_LENGTH, mask2, length * 2)) {
          score_table[addr] = i.score;
          score_type_table[addr] = i.type;
        }
      }
    }
  }
}

bool AlphaBetaAlgorithm::IsAddrContainsMask(int addr, int addrBitCount,
                                            int mask, int maskBitCount) {
  int k = 0;
  // create a temp number k with max binary number with length maskBitCount.
  for (int i = 0; i < maskBitCount; i++) k |= 1 << i;  // NOLINT
  // find target bit data in address number.
  for (int i = maskBitCount; i <= addrBitCount; i += 2) {
    if ((addr & k) == mask) return true;  // NOLINT
    mask <<= 2;                           // NOLINT
    k <<= 2;                              // NOLINT
  }
  return false;
}

bool AlphaBetaAlgorithm::IsValidPosition(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], int x, int y) {
  // check if there's an occupied grid within given position
  for (int dx = -SEARCH_RANGE; dx <= SEARCH_RANGE; dx++) {
    for (int dy = -SEARCH_RANGE; dy <= SEARCH_RANGE; dy++) {
      int new_x = x + dx;
      int new_y = y + dy;
      // if new coordinate is out of range, skip this grid
      if (new_x < 0 || new_x >= Game::BOARD_SIZE || new_y < 0 ||
          new_y >= Game::BOARD_SIZE) {
        continue;
      }
      // if current grid is occupied, condition is satisfied
      if (board[new_x][new_y] != Stone::EMPTY) return true;
    }
  }
  return false;
}

CandidatePosition* AlphaBetaAlgorithm::SearchCandidatePosition(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player) {
  CandidatePosition* pRoot = nullptr;
  // iterator through each grid on board
  for (int i = 0; i < Game::BOARD_SIZE; i++) {
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if current grid is empty and
      // its neighbor within search range is occupied
      if (board[i][j] == Stone::EMPTY && IsValidPosition(board, i, j)) {
        // temporarily place player stone
        board[i][j] = player;
        // calculate the score of current position
        int value = ScorePoint(board, player, i, j);
        // reset current grid back to empty
        board[i][j] = Stone::EMPTY;
        //  code below perform doubly linked list sort
        // ==========================================================
        // create a new node of candidate position
        auto* pnew = new CandidatePosition(i, j, value);
        // if the doubly linked list is empty, set current node to head
        if (pRoot == nullptr)
          pRoot = pnew;
        else {
          CandidatePosition* p = pRoot;
          while (true) {
            // if current node is greater than new node
            if (p->grid_value > value) {
              // if has next node, then walk to next node
              if (p->pNext != nullptr) p = p->pNext;
              // otherwise append to tail
              else {
                p->pNext = pnew;
                pnew->pPrev = p;
                break;
              }
            } else {
              // otherwise it means new node is greater than current
              // if current is head, append new node to head
              if (p->pPrev != nullptr) {
                p->pPrev->pNext = pnew;
                pnew->pPrev = p->pPrev;
                pnew->pNext = p;
                p->pPrev = pnew;
                break;
              } else {
                // otherwise insert new node in front of current node
                pnew->pPrev = nullptr;
                pnew->pNext = p;
                p->pPrev = pnew;
                pRoot = pnew;
                break;
              }
            }
          }
        }
        // ==========================================================
      }
    }
  }

  return pRoot;
}

int AlphaBetaAlgorithm::MinMax(Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE],
                               int depth, Stone maxPlayer, Stone player,
                               int alpha, int beta) {
  // if reach terminal state, return board state score
  if (depth == 0 || GetWinner(board) != 0) {
    return EvaluateMinMax(board, maxPlayer);
  }
  if (player == maxPlayer) {
    int bestValue = std::numeric_limits<int>::min();
    // perform sort for candidate position based on point value
    CandidatePosition* pRoot = SearchCandidatePosition(board, player);
    CandidatePosition* pNext = pRoot;
    // walk through doubly linked list to find best point within depth search
    while (pNext) {
      // retrieve x and y coordinate from current candidate position
      int x = pNext->row_index;
      int y = pNext->column_index;
      // temporarily place player stone
      board[x][y] = player;
      // perform minimax recursion to find global point value
      int value = MinMax(board, depth - 1, maxPlayer,
                         (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK,
                         alpha, beta);
      // reset current grid back to empty
      board[x][y] = Stone::EMPTY;
      // update alpha to current best value
      bestValue = max(bestValue, value);
      alpha = max(alpha, bestValue);
      // perform beta pruning
      if (beta <= alpha)
        // since candidate position is sorted, this means the rest
        // of the point must have lower score and thus can be halted
        break;
      // walk to next candidate node
      pNext = pNext->pNext;
    }
    // delete the doubly linked list
    pNext = pRoot;
    while (pNext) {
      CandidatePosition* ptemp = pNext->pNext;
      delete pNext;
      pNext = ptemp;
    }
    return bestValue;
  } else {
    int bestValue = std::numeric_limits<int>::max();
    // perform sort for candidate position based on point value
    CandidatePosition* pRoot = SearchCandidatePosition(board, player);
    CandidatePosition* pNext = pRoot;
    // walk through doubly linked list to find best point within depth search
    while (pNext) {
      // retrieve x and y coordinate from current candidate position
      int x = pNext->row_index;
      int y = pNext->column_index;
      // temporarily place player stone
      board[x][y] = player;
      // perform minimax recursion to find global point value
      int value = MinMax(board, depth - 1, maxPlayer,
                         (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK,
                         alpha, beta);
      // reset current grid back to empty
      board[x][y] = Stone::EMPTY;
      // update beta to current best value
      bestValue = min(bestValue, value);
      beta = min(beta, bestValue);
      // perform alpha pruning
      if (beta <= alpha)
        // since candidate position is sorted, this means the rest
        // of the point must have lower score and thus can be halted
        break;
      // walk to next candidate node
      pNext = pNext->pNext;
    }
    // delete the doubly linked list
    pNext = pRoot;
    while (pNext) {
      CandidatePosition* ptemp = pNext->pNext;
      delete pNext;
      pNext = ptemp;
    }
    return bestValue;
  }
}

int AlphaBetaAlgorithm::AlphaBetaGo(
    Stone chess[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player, int& x,
    int& y) {
  // check if it's the first stone in the game
  bool is_first = true;
  for (int i = 0; i < Game::BOARD_SIZE; i++) {
    for (int j = 0; j < Game::BOARD_SIZE; j++)
      if (chess[i][j] != Stone::EMPTY) {
        is_first = false;
        break;
      }
    if (!is_first) break;
  }
  // if yes, place the stone in the middle of the board
  if (is_first) {
    x = (int)(Game::BOARD_SIZE / 2);
    y = (int)(Game::BOARD_SIZE / 2);
    return 1;
  }
  // reset backup chess board.
  memcpy(board_backup, chess,
         Game::BOARD_SIZE * Game::BOARD_SIZE * sizeof(int));
  // calculate score for current board state
  // minimax just need to update score for attempt grid, much more efficient
  ScoreChessToCache(chess, Stone::BLACK, &black_score_cache);
  ScoreChessToCache(chess, Stone::WHITE, &white_score_cache);
  // using alpha-beta pruning to find best place to place stone
  int bestX = -1;
  int bestY = -1;
  int bestValue = std::numeric_limits<int>::min();
  // go through each grid on the board
  for (int i = 0; i < Game::BOARD_SIZE; i++) {
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if current grid is empty and its neighbor is within 2 grid range
      if (chess[i][j] == Stone::EMPTY && IsValidPosition(chess, i, j)) {
        // temporarily place player stone in current grid
        chess[i][j] = player;
        // using minimax to simulate play and find score of this grid
        int value = MinMax(
            chess, SEARCH_DEPTH, player,
            (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK,
            std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        // reset current grid back to empty
        chess[i][j] = Stone::EMPTY;
        // if current grid value is greater than max
        // or equal to current value (using 30% random disturbance)
        if (value > bestValue ||
            (value == bestValue && rand() % 100 < 30)) {  // NOLINT
          // update current best value and corresponding x and y coordinate
          bestValue = value;
          bestX = i;
          bestY = j;
        }
      }
    }
  }
  // if there's grid left then assign best value to x and y
  if (bestX != -1 && bestY != -1) {
    x = bestX;
    y = bestY;
    return 1;
  }
  // otherwise it means no grid is empty
  return 0;
}

int AlphaBetaAlgorithm::AlphaBetaGoMT(Stone (*board)[Game::BOARD_SIZE],
                                      Stone player, int& x, int& y) {
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
    x = (int)(Game::BOARD_SIZE / 2);
    y = (int)(Game::BOARD_SIZE / 2);
    return 1;
  }
  // reset backup chess board.
  memcpy(board_backup, board,
         Game::BOARD_SIZE * Game::BOARD_SIZE * sizeof(int));
  // calculate score for current board state
  // minimax just need to update score for attempt grid, much more efficient
  ScoreChessToCache(board, Stone::BLACK, &black_score_cache);
  ScoreChessToCache(board, Stone::WHITE, &white_score_cache);
  // using alpha-beta pruning to find best place to place stone
  int bestX = -1;
  int bestY = -1;
  int bestValue = std::numeric_limits<int>::min();
  // initialize thread parameter, pass game info into each thread
  HANDLE threadHandle[THREAD_NUM] = {};
  MinMaxThreadParam threadParam[THREAD_NUM];
  for (auto& i : threadParam) {
    memcpy(i.board, board, Game::BOARD_SIZE * Game::BOARD_SIZE * sizeof(int));
    i.pAlgorithm = this;
    i.depth = SEARCH_DEPTH;
    i.maxPlayer = player;
    i.player = (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
  }
  // go through each grid on the board
  for (int i = 0; i < Game::BOARD_SIZE; i++) {
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      if (board[i][j] == 0 && IsValidPosition(board, i, j)) {
        // the index of idle thread
        int index = -1;
        // iterate through each thread to find idle thread
        for (int k = 0; k < THREAD_NUM; k++) {
          if (threadHandle[k] == nullptr) {
            index = k;
            break;
          }
        }
        // if there is no idle thread
        if (index == -1) {
          DWORD ret =
              WaitForMultipleObjects(THREAD_NUM, threadHandle, false, INFINITE);
          switch (ret) {
            case WAIT_TIMEOUT:
            case WAIT_FAILED:
              break;
            default:
              // reset thread into null pointer, retrieve x and y coordinate
              index = ret - WAIT_OBJECT_0;
              CloseHandle(threadHandle[index]);
              threadHandle[index] = nullptr;
              if (threadParam[index].bestValue > bestValue ||
                  (threadParam[index].bestValue == bestValue &&
                   rand() % 100 < 30)) {
                bestValue = threadParam[index].bestValue;
                bestX = threadParam[index].x;
                bestY = threadParam[index].y;
              }
              break;
          }
        }
        // if there's idle thread
        if (index >= 0) {
          // pass coordinate that need to be simulated into this thread
          threadParam[index].x = i;
          threadParam[index].y = j;
          threadHandle[index] = CreateThread(nullptr, 0, MinMaxThread,
                                             &threadParam[index], 0, nullptr);
        }
      }
    }
  }
  // retrieve result from all thread, and close each thread
  for (int k = 0; k < THREAD_NUM; k++) {
    if (threadHandle[k] != nullptr) {
      WaitForSingleObject(threadHandle[k], INFINITE);
      CloseHandle(threadHandle[k]);
      if (threadParam[k].bestValue > bestValue ||
          (threadParam[k].bestValue == bestValue && rand() % 100 < 30)) {
        bestValue = threadParam[k].bestValue;
        bestX = threadParam[k].x;
        bestY = threadParam[k].y;
      }
    }
  }
  // if there's grid left then assign best value to x and y
  if (bestX != -1 && bestY != -1) {
    x = bestX;
    y = bestY;
    return 1;
  }
  // otherwise it means no grid is empty
  return 0;
}

void AlphaBetaAlgorithm::ScoreChessPointToCache(
    Stone chess[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player, int x, int y,
    ScoreCache* pCache) {
  int me = player;
  int opponent = me == Stone::BLACK ? Stone::WHITE : Stone::BLACK;
  // check row
  if (true) {
    int k = 0;           // binary number length count
    int addr = 0;        // score table address
    int maxValue = 0;    // current max value
    int maxType = NONE;  // current max type
    // go through each grid at current row
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if it's the player's stone, attach 01
      // if it's opponent's stone, attach 11
      // otherwise attach 00
      addr <<= 2;  // NOLINT
      k += 2;
      if (chess[j][y] == me) addr |= 1;        // NOLINT
      if (chess[j][y] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > maxValue) {
          // update max score
          maxValue = value;
          maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
    }
    // record score for current row
    pCache->horizontal_score[y] = maxValue;
    pCache->horizontal_type[y] = maxType;
  }
  // check column
  if (true) {
    int k = 0;           // binary number length count
    int addr = 0;        // score table address
    int maxValue = 0;    // current max value
    int maxType = NONE;  // current max type
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if it's the player's stone, append 01
      // if it's opponent's stone, append 11
      // otherwise append 00
      addr <<= 2;  // NOLINT
      k += 2;
      if (chess[x][j] == me) addr |= 1;        // NOLINT
      if (chess[x][j] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > maxValue) {
          // update max score
          maxValue = value;
          maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
    }
    // record score for current column
    pCache->vertical_score[x] = maxValue;
    pCache->vertical_type[x] = maxType;
  }
  // check diagonal
  if (true) {
    int k = 0;           // binary number length count
    int addr = 0;        // score table address
    int maxValue = 0;    // current max value
    int maxType = NONE;  // current max type
    // find starting x and y coordinate of current diagonal
    int start_x = 0, start_y = 0;
    if (x < y)
      start_y = y - x;
    else
      start_x = x - y;
    // find index of the diagonal in score table
    int index = (start_y == 0) ? start_x : Game::BOARD_SIZE + start_y - 1;
    // iterate through each grid at current diagonal
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if x or y exceed valid range then break
      if (start_x >= Game::BOARD_SIZE || start_y >= Game::BOARD_SIZE) break;
      addr <<= 2;  // NOLINT
      k += 2;
      if (chess[start_x][start_y] == me) addr |= 1;        // NOLINT
      if (chess[start_x][start_y] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > maxValue) {
          // update max score
          maxValue = value;
          maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
      start_x++;
      start_y++;
    }
    // record score for current diagonal
    pCache->diagonal_score[index] = maxValue;
    pCache->diagonal_type[index] = maxType;
  }
  // check anti-diagonal
  if (true) {
    int k = 0;           // binary number length count
    int addr = 0;        // score table address
    int maxValue = 0;    // current max value
    int maxType = NONE;  // current max type
    // find starting x and y coordinate of current diagonal
    int current_x = Game::BOARD_SIZE - 1, current_y = 0;
    if ((Game::BOARD_SIZE - 1 - x) < y)
      current_y = y - (Game::BOARD_SIZE - 1 - x);
    else
      current_x = x + y;
    // find index of the diagonal in score tablef
    int index =
        (current_y == 0) ? 18 - current_x : current_y + Game::BOARD_SIZE - 1;
    // iterate through each grid at current diagonal
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      if (current_x < 0 || current_y >= Game::BOARD_SIZE) break;
      addr <<= 2;  // NOLINT
      k += 2;
      if (chess[current_x][current_y] == me) addr |= 1;        // NOLINT
      if (chess[current_x][current_y] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > maxValue) {
          // update max score
          maxValue = value;
          maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
      current_x--;
      current_y++;
    }
    // record score for current anti-diagonal
    pCache->antiDiagonal_score[index] = maxValue;
    pCache->antiDiagonal_type[index] = maxType;
  }
}

int AlphaBetaAlgorithm::EvaluateMinMax(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone maxPlayer) {
  // create two ScoreCache for black and white stone
  ScoreCache tmpBlackScoreCache{};
  ScoreCache tmpWhiteScoreCache{};
  // make copy of global ScoreCache to temp ScoreCache
  memcpy(&tmpBlackScoreCache, &black_score_cache, sizeof(ScoreCache));
  memcpy(&tmpWhiteScoreCache, &white_score_cache, sizeof(ScoreCache));
  // for every temporary move
  for (int i = 0; i < Game::BOARD_SIZE; i++)
    for (int j = 0; j < Game::BOARD_SIZE; j++)
      if (board[i][j] != board_backup[i][j]) {
        ScoreChessPointToCache(board, Stone::BLACK, i, j, &tmpBlackScoreCache);
        ScoreChessPointToCache(board, Stone::WHITE, i, j, &tmpWhiteScoreCache);
      }
  // retrieve final score for black and white
  int black_max = ScoreChess(&tmpBlackScoreCache);
  int white_max = ScoreChess(&tmpWhiteScoreCache);
  // best score for current player equals my score minus opponent score
  if (maxPlayer == Stone::BLACK)
    return black_max - white_max;
  else
    return white_max - black_max;
}

int AlphaBetaAlgorithm::ScoreChess(ScoreCache* pCache) {
  int best_row_score = 0, best_row_type = 0;
  int best_column_score = 0, best_column_type = 0;
  int best_diagonal_score = 0, best_diagonal_type = 0;
  int best_antiDiagonal_score = 0, best_antiDiagonal_type = 0;
  // iterate through each row and column
  for (int i = 0; i < Game::BOARD_SIZE; i++) {
    // find best row type based on row score
    if (pCache->horizontal_score[i] > best_row_score) {
      best_row_score = pCache->horizontal_score[i];
      best_row_type = pCache->horizontal_type[i];
    }
    // find best column type based on column score
    if (pCache->vertical_score[i] > best_column_score) {
      best_column_score = pCache->vertical_score[i];
      best_column_type = pCache->vertical_type[i];
    }
  }
  // iterate through each diagonal and anti-diagonal
  for (int i = 0; i < 2 * Game::BOARD_SIZE - 1; i++) {
    // find best diagonal type based on diagonal score
    if (pCache->diagonal_score[i] > best_diagonal_score) {
      best_diagonal_score = pCache->diagonal_score[i];
      best_diagonal_type = pCache->diagonal_type[i];
    }
    // find best anti-diagonal type based on anti-diagonal score
    if (pCache->antiDiagonal_score[i] > best_antiDiagonal_score) {
      best_antiDiagonal_score = pCache->antiDiagonal_score[i];
      best_antiDiagonal_type = pCache->antiDiagonal_type[i];
    }
  }
  // find max value among row, column, diagonal, and anti-diagonal
  int value = max(best_row_score,
                  max(best_column_score,
                      max(best_diagonal_score, best_antiDiagonal_score)));
  // count number of occurrence of each type
  int consecutive_four = 0, open_three = 0, half_open_three = 0, open_two = 0,
      half_open_two = 0;
  switch (best_row_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  switch (best_column_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  switch (best_diagonal_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  switch (best_antiDiagonal_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  // score of combination of each type is retrieved from internet
  // not the best probably but works fine
  if (consecutive_four >= 2) return max(value, 10000);
  if (consecutive_four >= 1 && open_three >= 1) return max(value, 10000);
  if (open_three >= 2) return max(value, 5000);
  if (open_three >= 1 && half_open_three >= 1) return max(value, 1000);
  if (open_two >= 2) return max(value, 100);
  if (open_two >= 1 && half_open_two >= 1) return max(value, 10);
  // return final score
  return value;
}

int AlphaBetaAlgorithm::ScorePoint(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player, int x,
    int y) {
  // find best row, column, diagonal, anti-diagonal score in each direction
  int best_row_score = 0, best_row_type = 0;
  int best_column_score = 0, best_column_type = 0;
  int best_diagonal_score = 0, best_diagonal_type = 0;
  int best_antiDiagonal_score = 0, best_antiDiagonal_type = 0;
  best_row_score = ScorePointDir(board, player, 0, x, y, best_row_type);
  best_column_score = ScorePointDir(board, player, 1, x, y, best_column_type);
  best_diagonal_score =
      ScorePointDir(board, player, 2, x, y, best_diagonal_type);
  best_antiDiagonal_score =
      ScorePointDir(board, player, 3, x, y, best_antiDiagonal_type);
  // find max value among row, column, diagonal, and anti-diagonal
  int value = max(best_row_score,
                  max(best_column_score,
                      max(best_diagonal_score, best_antiDiagonal_score)));
  // count number of occurrence of each type
  int consecutive_four = 0, open_three = 0, half_open_three = 0, open_two = 0,
      half_open_two = 0;
  switch (best_row_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  switch (best_column_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  switch (best_diagonal_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  switch (best_antiDiagonal_type) {
    case OPEN_FOUR:
      consecutive_four++;
      break;
    case OPEN_THREE:
      open_three++;
      break;
    case HALF_OPEN_THREE:
      half_open_three++;
      break;
    case HALF_OPEN_TWO:
      half_open_two++;
      break;
    case OPEN_TWO:
      open_two++;
      break;
    default:
      break;
  }
  // score of combination of each type is retrieved from internet
  // not the best probably but works fine
  if (consecutive_four >= 2) return max(value, 10000);
  if (consecutive_four >= 1 && open_three >= 1) return max(value, 10000);
  if (open_three >= 2) return max(value, 5000);
  if (open_three >= 1 && half_open_three >= 1) return max(value, 1000);
  if (open_two >= 2) return max(value, 100);
  if (open_two >= 1 && half_open_two >= 1) return max(value, 10);
  // return final score
  return value;
}

int AlphaBetaAlgorithm::ScorePointDir(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player, int dir,
    int x, int y, int& type) {
  int me = player;
  int opponent = me == Stone::BLACK ? Stone::WHITE : Stone::BLACK;
  int max_value = 0;
  int max_type = NONE;
  // check row, column, diagonal, or anti-diagonal based on dir value
  if (dir == 0) {
    int k = 0;     // binary number length count
    int addr = 0;  // score table address
    // go through each grid at current row
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if it's the player's stone, attach 01
      // if it's opponent's stone, attach 11
      // otherwise attach 00
      addr <<= 2;  // NOLINT
      k += 2;
      if (board[j][y] == me) addr |= 1;        // NOLINT
      if (board[j][y] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > max_value) {
          // update max score
          max_value = value;
          max_type = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
    }
    // record score for current row
  } else if (dir == 1) {
    int k = 0;     // binary number length count
    int addr = 0;  // score table address
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if it's the player's stone, append 01
      // if it's opponent's stone, append 11
      // otherwise append 00
      addr <<= 2;  // NOLINT
      k += 2;
      if (board[x][j] == me) addr |= 1;        // NOLINT
      if (board[x][j] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > max_value) {
          // update max score
          max_value = value;
          max_type = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
    }
    // record score for current column
  } else if (dir == 2) {
    int k = 0;     // binary number length count
    int addr = 0;  // score table address
    // find starting x and y coordinate of current diagonal
    int start_x = 0, start_y = 0;
    if (x < y)
      start_y = y - x;
    else
      start_x = x - y;
    // iterate through each grid at current diagonal
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      // if x or y exceed valid range then break
      if (start_x >= Game::BOARD_SIZE || start_y >= Game::BOARD_SIZE) break;
      addr <<= 2;  // NOLINT
      k += 2;
      if (board[start_x][start_y] == me) addr |= 1;        // NOLINT
      if (board[start_x][start_y] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > max_value) {
          // update max score
          max_value = value;
          max_type = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
      start_x++;
      start_y++;
    }
  } else if (dir == 3) {
    int k = 0;     // binary number length count
    int addr = 0;  // score table address
    // find starting x and y coordinate of current diagonal
    int current_x = Game::BOARD_SIZE - 1, current_y = 0;
    if ((Game::BOARD_SIZE - 1 - x) < y)
      current_y = y - (Game::BOARD_SIZE - 1 - x);
    else
      current_x = x + y;
    // iterate through each grid at current diagonal
    for (int j = 0; j < Game::BOARD_SIZE; j++) {
      if (current_x < 0 || current_y >= Game::BOARD_SIZE) break;
      addr <<= 2;  // NOLINT
      k += 2;
      if (board[current_x][current_y] == me) addr |= 1;        // NOLINT
      if (board[current_x][current_y] == opponent) addr |= 2;  // NOLINT
      // if binary number length reach bit data length threshold
      // retrieve corresponding score from table
      if (k >= BIT_DATA_LENGTH) {
        int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
        if (value > max_value) {
          // update max score
          max_value = value;
          max_type = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
        }
      }
      current_x--;
      current_y++;
    }
  }
  // assign max type to type reference
  type = max_type;
  // return max value in current direction
  return max_value;
}

void AlphaBetaAlgorithm::ScoreChessToCache(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE], Stone player,
    ScoreCache* pCache) {
  int me = player;
  int opponent = me == Stone::BLACK ? Stone::WHITE : Stone::BLACK;
  // check horizontal
  if (true) {
    for (int i = 0; i < Game::BOARD_SIZE; i++) {
      int k = 0;           // binary number length count
      int addr = 0;        // score table address
      int maxValue = 0;    // current max value
      int maxType = NONE;  // current max type
      // go through each grid at current row
      for (int j = 0; j < Game::BOARD_SIZE; j++) {
        // if it's the player's stone, attach 01
        // if it's opponent's stone, attach 11
        // otherwise attach 00
        addr <<= 2;  // NOLINT
        k += 2;
        if (board[j][i] == me) addr |= 1;        // NOLINT
        if (board[j][i] == opponent) addr |= 2;  // NOLINT
        // if binary number length reach bit data length threshold
        // retrieve corresponding score from table
        if (k >= BIT_DATA_LENGTH) {
          int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
          if (value > maxValue) {
            // update max score
            maxValue = value;
            maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
          }
        }
      }
      // record score for current row
      pCache->horizontal_score[i] = maxValue;
      pCache->horizontal_type[i] = maxType;
    }
  }
  // check vertial
  if (true) {
    for (int i = 0; i < Game::BOARD_SIZE; i++) {
      int k = 0;           // binary number length count
      int addr = 0;        // score table address
      int maxValue = 0;    // current max value
      int maxType = NONE;  // current max type
      for (int j = 0; j < Game::BOARD_SIZE; j++) {
        // if it's the player's stone, append 01
        // if it's opponent's stone, append 11
        // otherwise append 00
        addr <<= 2;  // NOLINT
        k += 2;
        if (board[i][j] == me) addr |= 1;        // NOLINT
        if (board[i][j] == opponent) addr |= 2;  // NOLINT
        // if binary number length reach bit data length threshold
        // retrieve corresponding score from table
        if (k >= BIT_DATA_LENGTH) {
          int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
          if (value > maxValue) {
            // update max score
            maxValue = value;
            maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
          }
        }
      }
      // record score for current column
      pCache->vertical_score[i] = maxValue;
      pCache->vertical_type[i] = maxType;
    }
  }
  // check diagonal
  if (true) {
    // check for lower side of diagonal
    for (int i = 0; i < Game::BOARD_SIZE; i++) {
      int k = 0;           // binary number length count
      int addr = 0;        // score table address
      int maxValue = 0;    // current max value
      int maxType = NONE;  // current max type
      // find starting x and y coordinate of current diagonal
      int start_x = i, start_y = 0;
      // iterate through each grid at current diagonal
      for (int j = 0; j < Game::BOARD_SIZE; j++) {
        // if x or y exceed valid range then break
        if (start_x >= Game::BOARD_SIZE || start_y >= Game::BOARD_SIZE) break;
        addr <<= 2;  // NOLINT
        k += 2;
        if (board[start_x][start_y] == me) addr |= 1;        // NOLINT
        if (board[start_x][start_y] == opponent) addr |= 2;  // NOLINT
        // if binary number length reach bit data length threshold
        // retrieve corresponding score from table
        if (k >= BIT_DATA_LENGTH) {
          int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
          if (value > maxValue) {
            // update max score
            maxValue = value;
            maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
          }
        }
        start_x++;
        start_y++;
      }
      // record score for current diagonal
      pCache->diagonal_score[i] = maxValue;
      pCache->diagonal_type[i] = maxType;
    }
    // check for upper side of diagonal
    for (int i = 1; i < Game::BOARD_SIZE; i++) {
      int k = 0;           // binary number length count
      int addr = 0;        // score table address
      int maxValue = 0;    // current max value
      int maxType = NONE;  // current max type
      // find starting x and y coordinate of current diagonal
      int start_x = 0, start_y = i;
      // iterate through each grid at current diagonal
      for (int j = 0; j < Game::BOARD_SIZE; j++) {
        // if x or y exceed valid range then break
        if (start_x >= Game::BOARD_SIZE || start_y >= Game::BOARD_SIZE) break;
        addr <<= 2;  // NOLINT
        k += 2;
        if (board[start_x][start_y] == me) addr |= 1;        // NOLINT
        if (board[start_x][start_y] == opponent) addr |= 2;  // NOLINT
        // if binary number length reach bit data length threshold
        // retrieve corresponding score from table
        if (k >= BIT_DATA_LENGTH) {
          int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
          if (value > maxValue) {
            // update max score
            maxValue = value;
            maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
          }
        }
        start_x++;
        start_y++;
      }
      // record score for current diagonal
      pCache->diagonal_score[i + Game::BOARD_SIZE - 1] = maxValue;
      pCache->diagonal_type[i + Game::BOARD_SIZE - 1] = maxType;
    }
  }
  // check anti-diagonal
  if (true) {  // HV2
    // check for lower side of diagonal
    for (int i = 0; i < Game::BOARD_SIZE; i++) {
      int k = 0;           // binary number length count
      int addr = 0;        // score table address
      int maxValue = 0;    // current max value
      int maxType = NONE;  // current max type
      // find starting x and y coordinate of current diagonal
      int start_x = Game::BOARD_SIZE - 1 - i, start_y = 0;
      // iterate through each grid at current diagonal
      for (int j = 0; j < Game::BOARD_SIZE; j++) {
        // if x or y exceed valid range then break
        if (start_x < 0 || start_y >= Game::BOARD_SIZE) break;
        addr <<= 2;  // NOLINT
        k += 2;
        if (board[start_x][start_y] == me) addr |= 1;        // NOLINT
        if (board[start_x][start_y] == opponent) addr |= 2;  // NOLINT
        // if binary number length reach bit data length threshold
        // retrieve corresponding score from table
        if (k >= BIT_DATA_LENGTH) {
          int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
          if (value > maxValue) {
            // update max score
            maxValue = value;
            maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
          }
        }
        start_x--;
        start_y++;
      }
      // record score for current diagonal
      pCache->diagonal_score[i] = maxValue;
      pCache->diagonal_type[i] = maxType;
    }
    // check for upper side of diagonal
    for (int i = 1; i < Game::BOARD_SIZE; i++) {
      int k = 0;           // binary number length count
      int addr = 0;        // score table address
      int maxValue = 0;    // current max value
      int maxType = NONE;  // current max type
      // find starting x and y coordinate of current diagonal
      int start_x = Game::BOARD_SIZE - 1, start_y = i;
      // iterate through each grid at current diagonal
      for (int j = 0; j < Game::BOARD_SIZE; j++) {
        // if x or y exceed valid range then break
        if (start_x >= Game::BOARD_SIZE || start_y >= Game::BOARD_SIZE) break;
        addr <<= 2;  // NOLINT
        k += 2;
        if (board[start_x][start_y] == me) addr |= 1;        // NOLINT
        if (board[start_x][start_y] == opponent) addr |= 2;  // NOLINT
        // if binary number length reach bit data length threshold
        // retrieve corresponding score from table
        if (k >= BIT_DATA_LENGTH) {
          int value = score_table[addr & BIT_DATA_SIZE];  // NOLINT
          if (value > maxValue) {
            // update max score
            maxValue = value;
            maxType = score_type_table[addr & BIT_DATA_SIZE];  // NOLINT
          }
        }
        start_x++;
        start_y++;
      }
      // record score for current diagonal
      pCache->diagonal_score[i + Game::BOARD_SIZE - 1] = maxValue;
      pCache->diagonal_type[i + Game::BOARD_SIZE - 1] = maxType;
    }
  }
}

int AlphaBetaAlgorithm::GetWinner(
    Stone board[Game::BOARD_SIZE][Game::BOARD_SIZE]) {
  for (int x = 0; x < Game::BOARD_SIZE; x++) {
    for (int y = 0; y < Game::BOARD_SIZE; y++) {
      if (board[x][y] != 0 && Game::IsWin(board, x, y)) {
        return board[x][y];
      }
    }
  }
  return Stone::EMPTY;
}

DWORD WINAPI AlphaBetaAlgorithm::MinMaxThread(void* pParam) {
  auto* program = (MinMaxThreadParam*)pParam;
  // make temporary move
  program->board[program->x][program->y] = program->maxPlayer;
  // minimax recursion to find best value
  program->bestValue = program->pAlgorithm->MinMax(
      program->board, program->depth, program->maxPlayer, program->player,
      std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
  // reset this grid back to empty
  program->board[program->x][program->y] = Stone::EMPTY;
  return 0;
}
