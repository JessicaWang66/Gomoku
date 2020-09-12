// Copyright (c) 2020 Yucheng Jiang. All rights reserved.

#include "my_app.h"

#include <cinder/app/App.h>
#include <mylibrary/MiniMax.h>
#include <mylibrary/SimpleAutoPlayer.h>

#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace cinder;
namespace myapp {
void MyApp::setup() {
  // set random seed for current game.
  srand(time(nullptr));  // NOLINT
  std::string path =
      R"(C:\App\Cinder\my_project\final-project-Yucheng-Jiang\assets\)";
  // set board board texture
  mTextureBoard = gl::Texture2d::create(loadImage(path + "chessboard.png"));
  // set white stone texture
  mTextureWhite = gl::Texture2d::create(loadImage(path + "white.png"));
  // set black stone texture
  mTextureBlack = gl::Texture2d::create(loadImage(path + "black.png"));
  // set windows size
  setWindowSize(mTextureBoard.get()->getSize());
  // set mouse position
  mShowPosX = -1;
  mShowPosY = -1;
  // select black side player (initialized to be human player)
  mBlackSelection = 0;
  mBlackPlayers = {"Player", "Random", "MinMax", "MinMaxMT", "Simple AI"};
  // select white side player (initialized to be minimax auto player)
  mWhiteSelection = 4;
  mWhitePlayers = {"Player", "Random", "MinMax", "MinMaxMT", "Simple AI"};
  // set up control panel (to select player type, restart/exit game)
  mParams = params::InterfaceGl::create(getWindow(), "Control",
                                        toPixels(ivec2(200, 400)));
  mParams->setPosition(vec2(OFFSETX + 20 * SIZEXY - 20, OFFSETY + 300));
  mParams->addText("Select Players", "");  // give user hint
  mParams->addSeparator("", "");           // add line separator
  mParams->addParam("Black Player", mBlackPlayers, &mBlackSelection);
  mParams->addParam("White Player", mWhitePlayers, &mWhiteSelection);
  mParams->addSeparator("");
  mParams->addButton(
      "Restart Game", [&]() { onRestart(); }, "key=n");
  mParams->addButton(
      "Exit Game", [&]() { onExit(); }, "key=x");
  mParams->setSize(vec2(200, 400));
  // set message box
  mMessage = params::InterfaceGl::create(getWindow(), "Message",
                                         toPixels(ivec2(200, 400)));
  mMessage->setPosition(vec2(OFFSETX + 20 * SIZEXY - 20, OFFSETY + 70));
  mMessage->addText("GOOD LUCK", "");
  mMessage->setSize(vec2(200, 200));
  // initialize latest places stone flash count.
  mFlashCount = 0;
}
void MyApp::onRestart() { game.Reset(); }
void MyApp::onExit() { exit(0); }
void MyApp::mouseMove(MouseEvent event) {
  // convert mouse position to board grid index
  int row_index = (int)((double)event.getPos().x - OFFSETX) / SIZEXY;
  int column_index = (int)((double)event.getPos().y - OFFSETY) / SIZEXY;
  bool should_show_cursor;
  // if current player is human player, show cursor for the user.
  if (game.GetRole() == 1 && mBlackSelection == 0)
    should_show_cursor = true;
  else
    should_show_cursor = game.GetRole() == 2 && mWhiteSelection == 0;
  // if given position is within range, update mouse position
  // otherwise reset to default value: -1
  if (should_show_cursor && row_index >= 0 && row_index < Game::BOARD_SIZE &&
      column_index >= 0 && column_index < Game::BOARD_SIZE) {
    mShowPosX = row_index;
    mShowPosY = column_index;
  } else {
    mShowPosX = -1;
    mShowPosY = -1;
  }
}
void MyApp::mouseDown(MouseEvent event) {
  // convert mouse position to board grid index
  int row_index = (int)((double)event.getPos().x - OFFSETX) / SIZEXY;
  int column_index = (int)((double)event.getPos().y - OFFSETY) / SIZEXY;
  // if mouse position is within range and if current in human player
  // place the stone at given position and flash to indicate updated position.
  if (row_index >= 0 && row_index < Game::BOARD_SIZE && column_index >= 0 &&
      column_index < Game::BOARD_SIZE) {
    if ((game.GetRole() == Stone::BLACK && mBlackSelection == 0) ||
        (game.GetRole() == Stone::WHITE && mWhiteSelection == 0)) {
      game.Play(row_index, column_index);
    }
    mFlashX = row_index;
    mFlashY = column_index;
    mFlashCount = 75;
  }
}
void MyApp::update() {
  // if currently does not need to flash latest stone
  if (mFlashCount == 0) {
    int x, y;
    // give auto player based on current game role
    switch (game.GetRole()) {
      case Stone::BLACK:
        // given auto play based on player type.
        switch (mBlackSelection) {
          case 0:  // human player doesn't need auto play
            break;
          case 1:                                  // random
            game.Play(rand() % Game::BOARD_SIZE,   // NOLINT
                      rand() % Game::BOARD_SIZE);  // NOLINT
            break;
          case 2:  // MinMax
            if (AlphaBeta.AlphaBetaGo(game.mChessStatus, Stone::BLACK, x, y) >
                0) {
              game.Play(x, y);
              mFlashX = x;
              mFlashY = y;
              mFlashCount = 75;
            } else
              game.Reset();
            break;
          case 3:  // MinMax multiple thread
            if (AlphaBeta.AlphaBetaGoMT(game.mChessStatus, Stone::BLACK, x, y) >
                0) {
              game.Play(x, y);
              mFlashX = x;
              mFlashY = y;
              mFlashCount = 75;
            } else
              game.Reset();
            break;
          case 4:  // simple auto player
            if (SimpleAutoPlayer::SimpleStrategy(game.mChessStatus,
                                                 Stone::BLACK, x, y) > 0) {
              game.Play(x, y);
              mFlashX = x;
              mFlashY = y;
              mFlashCount = 75;
            } else
              game.Reset();
            break;
        }
        break;
      case Stone::WHITE:
        // given auto play based on player type.
        switch (mWhiteSelection) {
          case 0:  // human player doesn't need auto play
            break;
          case 1:                                  // random
            game.Play(rand() % Game::BOARD_SIZE,   // NOLINT
                      rand() % Game::BOARD_SIZE);  // NOLINT
            break;
          case 2:  // MinMax
            if (AlphaBeta.AlphaBetaGo(game.mChessStatus, Stone::WHITE, x, y) >
                0) {
              game.Play(x, y);
              mFlashX = x;
              mFlashY = y;
              mFlashCount = 75;
            } else
              game.Reset();
            break;
          case 3:  // MinMax multiple thread
            if (AlphaBeta.AlphaBetaGoMT(game.mChessStatus, Stone::WHITE, x, y) >
                0) {
              game.Play(x, y);
              mFlashX = x;
              mFlashY = y;
              mFlashCount = 75;
            } else
              game.Reset();
            break;
          case 4:  // simple auto player
            if (SimpleAutoPlayer::SimpleStrategy(game.mChessStatus,
                                                 Stone::BLACK, x, y) > 0) {
              game.Play(x, y);
              mFlashX = x;
              mFlashY = y;
              mFlashCount = 75;
            } else
              game.Reset();
            break;
        }
        break;
      default:
        break;
    }
  }
}
void MyApp::draw() {
  gl::clear();
  // draw chess board
  gl::draw(mTextureBoard);
  // draw chess pieces
  for (int x = 0; x < Game::BOARD_SIZE; x++) {
    for (int y = 0; y < Game::BOARD_SIZE; y++) {
      // if current grid does not need to flash
      if (x != mFlashX || y != mFlashY || mFlashCount <= 0 ||
          (mFlashCount % 50) <= 25) {
        // draw corresponding stone if it is black or white
        switch (game.GetStatus(x, y)) {
          case Stone::BLACK:
            gl::draw(mTextureBlack,
                     vec2(OFFSETX + x * SIZEXY, OFFSETY + y * SIZEXY));
            break;
          case Stone::WHITE:
            gl::draw(mTextureWhite,
                     vec2(OFFSETX + x * SIZEXY, OFFSETY + y * SIZEXY));
            break;
          default:
            break;
        }
      }
    }
  }
  // count down the flash number
  if (mFlashCount > 0) mFlashCount--;
  // show current player type on the right upper corner of the window
  switch (game.GetRole()) {
    case Stone::BLACK:
      gl::draw(mTextureBlack,
               vec2(OFFSETX + 21 * SIZEXY + 10, OFFSETY + SIZEXY / 2));
      break;
    case Stone::WHITE:
      gl::draw(mTextureWhite,
               vec2(OFFSETX + 21 * SIZEXY + 10, OFFSETY + SIZEXY / 2));
      break;
    // if it's empty, it means there's a winner
    default:
      // show the winner
      drawWinner(game.mWinner);
  }
  // if need to show cursor, draw a high lighted white circle
  if (mShowPosX >= 0 && mShowPosY >= 0)
    gl::drawStrokedCircle(vec2(OFFSETX + Game::BOARD_SIZE + mShowPosX * SIZEXY,
                               OFFSETY + Game::BOARD_SIZE + mShowPosY * SIZEXY),
                          10);
  // update message box
  mMessage->draw();
  // update parameter box
  mParams->draw();
}
void MyApp::drawWinner(Stone winner) {
  static float fontSize = 32;
  static float fontSizePlus = 8;
  // show black wins
  fontSize += fontSizePlus;
  if (fontSize > 180)
    fontSizePlus = -8;
  else if (fontSize < 32)
    fontSizePlus = 8;
  switch (winner) {
    case Stone::BLACK:
      gl::drawStringCentered("BLACK WIN", vec2(400, 300), ColorA(0, 0, 0, 0.8),
                             Font("", fontSize));
      break;
    case Stone::WHITE:
      gl::drawStringCentered("White WIN", vec2(400, 300), ColorA(1, 1, 1, 0.8),
                             Font("", fontSize));
      break;
    default:
      gl::drawStringCentered("Game DRAW", vec2(400, 300), ColorA(1, 1, 1, 0.8),
                             Font("", fontSize));
  }
}
}  // namespace myapp
