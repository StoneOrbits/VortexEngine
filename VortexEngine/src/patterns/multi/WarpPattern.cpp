#include "WarpPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

WarpPattern::WarpPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_progress()
{
}

WarpPattern::~WarpPattern()
{
}

// init the pattern to initial state
void WarpPattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void WarpPattern::blinkOn()
{
  Leds::setAll(m_colorset.cur());
  Leds::setFinger((Finger)m_progress, m_colorset.peekNext());
}

void WarpPattern::poststep()
{
  m_progress = (m_progress + 1) % FINGER_COUNT;
  if (m_progress == 0) {
    m_colorset.getNext();
  }
}