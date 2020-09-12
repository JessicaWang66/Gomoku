# Gomoku

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](README.md)

**Author**: Yucheng Jiang - [`yj17@illinois.edu`](mailto:example@illinois.edu)


> Gomoku (five-in-a-row) game featuring a powerful Alpha-beta pruning algorithm written in `C++`. Download the latest release executable file  <a href="https://github.com/CS126SP20/final-project-Yucheng-Jiang/releases/download/v1.0/gomuku.exe" target="_blank">here</a>. 

---

## Table of Contents

- [Installation](#installation)
- [Algorithm](#algorithm)
- [Contributing](#contributing)

---

## Installation

### Clone

- Clone this repo to your local machine using `https://github.com/CS126SP20/final-project-Yucheng-Jiang.git`

### Setup

> Clone imGUI from `https://github.com/simongeilfus/Cinder-ImGui` and save the file under `{cinder root file path}\blocks\imGUI`

> Now you are ready to run the program

> If want to make some changes to the program, here are a few things that can be adjusted
>
> > **Change game rule** in `\include\Game.h`
> >
> > ```c++
> > class Game {
> >  public:
> >   // standard board size.
> >   static const int BOARD_SIZE = 19;
> >   // number of consecutive stone needed to win
> >   static const int WINNING_THRESHOLD = 5;
> >   // current winner
> >   Stone mWinner;
> >   ....
> >       
> >  public:
> >     .....
> >  private:
> >     .....
> > }
> > ```
> >
> > Make changes to `BOARD_SIZE` to adjust board size. Note that currently only support board size from 5 to 19 (inclusive). Also the user interface not yet support board size change.
> >
> > Make changes to WINNING_THRESHOLD to adjust winning condition for the game. Adjust to 3 to make the game to become TIC_TAC_TOE.
>
> > **Change Alpha-Beta pruning algorithm parameters** in `\include\Minimax.h`
> >
> > ```c++
> > // we only care about 7 consecutive stones on the board
> > // each stone convert to 2 bit number, 00 , 01, or 11
> > // therefore, max size would be 2^(7 * 2) = 0x3fff
> > static const int BIT_DATA_LENGTH = 14;
> > static const int BIT_DATA_SIZE = 0x3fff;  // NOLINT
> > // search for empty grid within 2 unit of the occupied grid.
> > // increase this value could slightly increase win rate
> > // but exponentially decrease efficiency
> > static const int SEARCH_RANGE = 2;
> > // default search depth. Increase depth will significantly increase win rate
> > // while factorial increase time needed to finish computation
> > static const int SEARCH_DEPTH = 3;
> > // multiple-thread number, default set to be 4. No harm to be higher.
> > static const int THREAD_NUM = 4;
> > 
> > enum PatternType{...}
> > struct Pattern{...}
> > struct ScoreCache{...}
> > class CandidatePosition{...}
> > class AlphaBetaAlgorithm{...}
> > struct MinimaxThreadParagm{...}
> > ```
> >
> > Make changes to SEARCH_RANGE will change the empty grid within N grid of the occupied position. Increase this value will significantly increase running time will have slightly higher winning rate.
> >
> > Make changes to SEARCH_DEPTH will change recursion depth in minimax algorithm. Increase this value will significantly increase both winning rate and running time. Ideally, the algorithm becomes unbeatably when the depth is greater than 5. 
> >
> > Make changes to THREAD_NUM will change number of thread used when MiniMaxMT strategy is  called. No harm to change the value larger than 4.
>
> > **Customized player strategy**, here are a few things that need to be changed
> >
> > First add your strategy name in `\apps\my_app.cc`
> >
> > ```c++
> > void MyApp::setup() {
> > // set random seed for current game.
> >   ......
> >   // select black side player (initialized to be human player)
> >   mBlackSelection = 0;
> >   mBlackPlayers = {"Player", "Random", "MinMax", "MinMaxMT"};
> >   // select white side player (initialized to be minimax auto player)
> >   mWhiteSelection = 3;
> >   mWhitePlayers = {"Player", "Random", "MinMax", "MinMaxMT"};
> >   // set up control panel (to select player type, restart/exit game)
> >   ......
> > }
> > ```
> >
> > Add or delete black and/or white player strategy in array `mBlackPlayers` and/or `mWhitePlayers`. Also change default strategy by changing index `mBlackSelection` and/or `mWhiteSelection` 
> >
> > Secondly, write your strategy in a new file. Place header file under `\include` folder and source file under `\src` folder.
> >
> > Finally, add control logic in `\apps\my_app.cc`
> >
> > ```c++
> > void MyApp::update() {
> >   // if currently does not need to flash latest stone
> >   if (mFlashCount == 0) {
> >     .....
> >     switch (game.GetRole()) {
> >       case Stone::BLACK:
> >         // given auto play based on player type.
> >         switch (mBlackSelection) {
> >           case 0:  // human player doesn't need auto play
> >             break;
> >           case 1:  // random
> >             ....
> >             break;
> >           case 2:  // MinMax
> >             ...
> >             break;
> >           case 3:  // MinMax multiple thread
> >             .....
> >             break;
> >           case 4: // your straegy here
> >             .....
> >             break;
> >         }
> >         break;
> >       case Stone::WHITE:
> >         // same as case Stone::BLACK above.
> >     }
> >   }
> > }
> > ```
> >
> > Add your call to your strategy in switch logic as shown above.
> >
> > Now run the app, you can choose your strategy on the right side of the screen.
>
> 

---

## Algorithm

1. Call `IniScoreTable` to set up score table for GoMoKu
2. Call `AlphabetaGo` or `AlphabetaGoMT` to get best move
   - Evaluate score for current board by calling `SchoreChessToCache`
   - For every available position (empty grid within SEARCH_RANGE)
     - try temporary move in this position
     - call `MiniMax` recursive function to retrieve score for this position
       - if reach terminal state (depth equal to 0 or has winner), call `EvaluateBoard`
         - for every grid that has temporary move, evaluate the move (`ScoreChessPointtoCache`)
         - retrieve score for the whole board (`ScoreChess`)
         - final score equals my score minus opponent score
       - if current player is max player
         - set best value to negative infinity
         - for all available position (empty grid within SEARCH_RANGE) sorted by point score (`SearchCandidatePosition`)
           - try move, call `minimax` to retrieve score for this point, and reset position to empty
           - update best value, and do beta pruning
       - if current player is not max player
         - set best value to positive infinity
         - for all available position (empty grid within SEARCH_RANGE) sorted by point score (`SearchCandidatePosition`)
           - try move, call `minimax` to retrieve score for this point, and reset position to empty
           - update best value, and do alpha pruning
     - reset current position back to empty
   - Return position with highest value

---

## Contributing

### Step 1

- **Option 1**
    - 🍴 Fork this repo!

- **Option 2**
    - 👯 Clone this repo to your local machine using `https://github.com/CS126SP20/final-project-Yucheng-Jiang.git`

### Step 2

- **HACK AWAY!** 🔨🔨🔨

### Step 3

- 🔃 pull request not yet supported

---

