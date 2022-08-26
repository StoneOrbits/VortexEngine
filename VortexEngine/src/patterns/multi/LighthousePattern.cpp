#include "LighthousePattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

LighthousePattern::LighthousePattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  BlinkStepPattern(2, 7, 100),
  m_fadeTimer(),
  m_stash(),
  m_progress(0)
{
}

LighthousePattern::~LighthousePattern()
{
}

// init the pattern to initial state
void LighthousePattern::init()
{
  BlinkStepPattern::init();

  // fade timer setup
  m_fadeTimer.reset();
  m_fadeTimer.addAlarm(5);
  m_fadeTimer.start();

  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

// pure virtual must override the play function
void LighthousePattern::play()
{
  if (m_fadeTimer.alarm() == 0) {
    fade();
  }

  BlinkStepPattern::play();
}

void LighthousePattern::blinkOn()
{
  Leds::restoreAll(m_stash);
  Leds::setIndex((LedPos)m_progress, m_colorset.cur());
}

void LighthousePattern::blinkOff()
{
  Leds::stashAll(m_stash);
  Leds::clearAll();
}

void LighthousePattern::poststep()
{
  m_progress = (m_progress + 1) % LED_COUNT;
  m_colorset.getNext();
}

void LighthousePattern::fade()
{
  Leds::adjustBrightnessAll(25);
}
