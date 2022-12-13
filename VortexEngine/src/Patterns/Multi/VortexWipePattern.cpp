#include "VortexWipePattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

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

VortexWipePattern::VortexWipePattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_progress(0)
{
  m_patternID = PATTERN_VORTEXWIPE;
}

VortexWipePattern::VortexWipePattern(const PatternArgs &args) :
  VortexWipePattern()
{
  setArgs(args);
}

VortexWipePattern::~VortexWipePattern()
{
}

// init the pattern to initial state
void VortexWipePattern::init()
{
  BlinkStepPattern::init();
  // reset progress
  m_progress = 0;
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
