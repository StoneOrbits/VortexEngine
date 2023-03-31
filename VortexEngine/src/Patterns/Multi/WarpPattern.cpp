#include "WarpPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

WarpPattern::WarpPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_progress(0)
{
  m_patternID = PATTERN_WARP;
}

WarpPattern::WarpPattern(const PatternArgs &args) :
  WarpPattern()
{
  setArgs(args);
}

WarpPattern::~WarpPattern()
{
}

// init the pattern to initial state
void WarpPattern::init()
{
  BlinkStepPattern::init();
  // reset progress
  m_progress = 0;
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void WarpPattern::blinkOn()
{
  Leds::setAll(m_colorset.cur());
  Leds::setPair((Pair)m_progress, m_colorset.peekNext());
}

void WarpPattern::poststep()
{
  m_progress = (m_progress + 1) % PAIR_COUNT;
  if (m_progress == 0) {
    m_colorset.getNext();
  }
}
