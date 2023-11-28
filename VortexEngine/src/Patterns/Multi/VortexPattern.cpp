#include "VortexPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

VortexPattern::VortexPattern(const PatternArgs& args) :
  BlinkStepPattern(args),
  m_progress(0)
{
  m_patternID = PATTERN_VORTEXWIPE;
  setArgs(args);
}

VortexPattern::~VortexPattern()
{
}

// init the pattern to initial state
void VortexPattern::init()
{
  BlinkStepPattern::init();
  // reset progress
  m_progress = 0;
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void VortexPattern::blinkOn()
{
  // Sets an LED at opposite ends of the strip and progresses towards the center
  Leds::setIndex((LedPos)m_progress, m_colorset.peekNext());
  Leds::setIndex((LedPos)(LED_LAST - m_progress), m_colorset.peekNext());
}

void VortexPattern::poststep()
{
  m_progress = (m_progress + 1) % (LED_COUNT/2);
  if (m_progress == 0) {
    m_colorset.getNext();
  }
}
