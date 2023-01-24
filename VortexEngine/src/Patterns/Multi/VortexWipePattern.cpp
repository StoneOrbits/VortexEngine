#include "VortexWipePattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

const LedPair VortexWipePattern::ledStepPairs[] = {
  PAIR_1,
  PAIR_5,
  PAIR_9,
  PAIR_13,
  PAIR_2,
  PAIR_6,
  PAIR_10,
  PAIR_14,
  PAIR_3,
  PAIR_7,
  PAIR_11,
  PAIR_15,
  PAIR_4,
  PAIR_8,
  PAIR_12,
  PAIR_16
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
    Leds::setPair(ledStepPairs[index], m_colorset.peekNext());
  }
  for (int index = m_progress; index < PAIR_COUNT; ++index) {
    Leds::setPair(ledStepPairs[index], m_colorset.cur());
  }
}

void VortexWipePattern::poststep()
{
  m_progress = (m_progress + 1) % PAIR_COUNT;
  if (m_progress == 0) {
    m_colorset.getNext();
  }
}
