#include "Game.h"

#include "../../Time/TimeControl.h"
#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

Game::Game() :
  Menu(),
  m_balls(nullptr),
  m_lastBall(nullptr),
  m_numBalls(0),
  m_hueSelect(0),
  m_gameTimer()
{
}

bool Game::init()
{
  if (!Menu::init()) {
    return false;
  }
  createBall();
  m_gameTimer.addAlarm(100);
  m_gameTimer.start();
  DEBUG_LOG("Welcome to game");
  return true;
}

bool Game::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // only run the game every gametimer tick
  if (m_gameTimer.alarm() == 0) {
    playGame();
  }

  // continue
  return true;
}

void Game::onShortClick()
{
  // if the last ball hasn't moved yet then release it
  if (m_lastBall) {
    if (m_lastBall->m_speed == 0) {
      m_lastBall->m_speed = 1;
      m_lastBall = nullptr;
      return;
    }
  }

  // otherwise create a new ball
  createBall();
}

void Game::onLongClick()
{
}

void Game::showMenu()
{
  Leds::clearAll();
  // gradually fill from thumb to pinkie
  LedPos pos = (LedPos)(LED_COUNT - (Time::getCurtime() / Time::msToTicks(100) % (LED_COUNT + 1)));
  if (pos == 10) return;
  Leds::setIndex(pos, RGB_TEAL);
}

void Game::playGame()
{
  Leds::clearAll();
  for (uint32_t i = 0; i < m_numBalls; ++i) {
    if (!m_balls[i]) {
      continue;
    }
    m_balls[i]->step();
  }
  // check the first ball for too many bounces
  if (m_balls && m_balls[0] && m_balls[0]->m_bounces > 25) {
    delete m_balls[0];
    m_balls[0] = nullptr;
    m_numBalls--;
    if (m_numBalls > 0) {
      memmove(m_balls, m_balls + 1, m_numBalls * sizeof(Game::Ball *));
      void *temp = realloc(m_balls, m_numBalls * sizeof(Game::Ball *));
      if (!temp) {
        ERROR_OUT_OF_MEMORY();
        return;
      }
      m_balls = (Game::Ball **)temp;
    } else {
      free(m_balls);
      m_balls = nullptr;
    }
  }
  if (m_lastBall && !m_lastBall->m_speed) {
    // blend the color of last ball through rainbow
    m_lastBall->m_color = HSVColor(m_hueSelect++, 255, 210);
  }
}

void Game::quitGame()
{
  leaveMenu();
}

bool Game::createBall()
{
  void *temp = realloc(m_balls, (m_numBalls + 1) * sizeof(Ball *));
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_balls = (Ball **)temp;
  Ball *newBall = new Ball();
  if (!newBall) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  // we now have more balls
  m_balls[m_numBalls] = newBall;
  m_numBalls++;
  // store the last ball
  m_lastBall = newBall;
  return true;
}

Game::Ball::Ball() :
  m_position(0),
  m_color(),
  m_speed(0),
  m_bounces(0)
{
}

void Game::Ball::step()
{
  if (m_position <= LED_LAST && m_position >= LED_FIRST) {
    Leds::setIndex((LedPos)m_position, m_color);
  }
  m_position += m_speed;
  if (m_position >= LED_LAST || m_position <= LED_FIRST && m_speed) {
    m_speed *= -1;
    m_bounces++;
  }
}
