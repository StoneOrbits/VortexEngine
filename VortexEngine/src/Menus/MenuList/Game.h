#ifndef GAME_H
#define GAME_H

#include "../Menu.h"

#include "../../Time/Timer.h"

class Game : public Menu
{
public:
  Game();

  bool init();
  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  // internal routines
  void showMenu();
  void playGame();
  void quitGame();

  class Ball {
    friend class Game;
  public:
    Ball();
    void step();
  private:
    int8_t m_position;
    RGBColor m_color;
    int8_t m_speed;
    uint8_t m_bounces;
  };

  bool createBall();

  // yo don't touch m balls bro
  Ball **m_balls;
  // pointer to my last ball
  Ball *m_lastBall;
  // how many balls I've got
  uint32_t m_numBalls;

  // the hue selector
  uint8_t m_hueSelect;

  // Timer for ball ticks
  Timer m_gameTimer;
};

#endif
