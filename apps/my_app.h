// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/app/KeyEvent.h>
#include <mylibrary/Game.h>
#include <mylibrary/MiniMax.h>

#include <chrono>
#include <vector>

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

namespace myapp {
using namespace ci;
using namespace ci::app;
using namespace std::chrono;

const int OFFSETX = 35;  // board starting x coordinate
const int OFFSETY = 14;  // board starting y coordinate
const int SIZEXY = 39;   // grid size

class MyApp : public App {
 public:
  /**
   * initialize user interface for the game.
   * Load boad and stone image, set parameter and message window
   * Initialize black and white side player
   */
  void setup() override;
  /**
   * update mouse position and place stones when left click.
   * @param event mouse event.
   */
  void mouseDown(MouseEvent event) override;
  /**
   * show highlighted cursor when human player's turn
   * @param event mouse event
   */
  void mouseMove(MouseEvent event) override;
  /**
   * update board status and call auto player (if selected)
   */
  void update() override;
  /**
   * draw new board.
   */
  void draw() override;
  /**
   * restart game.
   */
  void onRestart();
  /**
   * exit program
   */
  static void onExit();

 private:
  /**
   * print the winner info on the screen.
   * @param winner winner stone type.
   */
  static void drawWinner(Stone winner);

 private:
  Game game;                     // new game instance
  AlphaBetaAlgorithm AlphaBeta;  // new alpha-beta algorithm instance

  gl::TextureRef mTextureBoard;     // board texture
  gl::TextureRef mTextureWhite;     // white stone texture
  gl::TextureRef mTextureBlack;     // black stone texture
  params::InterfaceGlRef mParams;   // parameter window
  params::InterfaceGlRef mMessage;  // message window

  int mShowPosX;    // highlighted circle x coordinate
  int mShowPosY;    // highlighted circle y coordinate
  int mFlashX;      // latest placed stone x coordinate
  int mFlashY;      // latest placed stone y coordinate
  int mFlashCount;  // latest placed stone flash count

  std::vector<std::string> mBlackPlayers;  // black side player choice list
  int mBlackSelection;                     // black side player choice index
  std::vector<std::string> mWhitePlayers;  // white side player choice list
  int mWhiteSelection;                     // white side player choice index
};
}  // namespace myapp
#endif
