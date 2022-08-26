#include "BouncePattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

#define TOTAL_STEPS ((FINGER_COUNT * 2) - 2)
#define HALF_STEPS (TOTAL_STEPS / 2)

BouncePattern::BouncePattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  BlinkStepPattern(3, 12, 50),
  m_progress()
{
}

BouncePattern::~BouncePattern()
{
}

// init the pattern to initial state
void BouncePattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void BouncePattern::blinkOn()
{
  Leds::setAll(m_colorset.cur());
  if (m_progress < FINGER_COUNT) {
    Leds::setFinger((Finger)m_progress, m_colorset.peekNext());
  } else {
    Leds::setFinger((Finger)(TOTAL_STEPS - m_progress), m_colorset.peekNext());
  }
}

void BouncePattern::poststep()
{
  m_progress = (m_progress + 1) % TOTAL_STEPS;
  if (m_progress == 0 || m_progress == HALF_STEPS) {
    m_colorset.getNext();
  }
}