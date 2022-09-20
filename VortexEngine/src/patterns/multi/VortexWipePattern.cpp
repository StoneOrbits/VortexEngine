#include "VortexWipePattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"
#include "../../Timings.h"

const LedPos VortexWipePattern::ledStepPositions[] = {
  THUMB_TOP,
  INDEX_TOP,
  MIDDLE_TOP,
  RING_TOP,
  PINKIE_TOP,

  PINKIE_TIP,
  RING_TIP,
  MIDDLE_TIP,
  INDEX_TIP,
  THUMB_TIP
};

VortexWipePattern::VortexWipePattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  BlinkStepPattern(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 125),
  m_progress()
{
}

VortexWipePattern::~VortexWipePattern()
{
}

// init the pattern to initial state
void VortexWipePattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void VortexWipePattern::blinkOn()
{
  for (int index = 0; index < m_progress; ++index) {
    Leds::setIndex(ledStepPositions[index], m_colorset.peekNext());
  }
  for (int index = m_progress; index < LED_COUNT; ++index) {
    Leds::setIndex(ledStepPositions[index], m_colorset.cur());
  }
}

void VortexWipePattern::poststep()
{
  m_progress = (m_progress + 1) % LED_COUNT;
  if (m_progress == 0) {
    m_colorset.getNext();
  }
}
