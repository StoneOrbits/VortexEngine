#include "MateriaPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

MateriaPattern::MateriaPattern(const PatternArgs& args) :
  BlinkStepPattern(args),
  m_progress(0)
{
  m_patternID = PATTERN_VORTEXWIPE;
  setArgs(args);
}

MateriaPattern::~MateriaPattern()
{
}

// init the pattern to initial state
void MateriaPattern::init()
{
  BlinkStepPattern::init();
  // reset progress
  m_progress = 0;
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void MateriaPattern::blinkOn()
{
  Leds::setRing((Ring)(RING_COUNT - m_progress), m_colorset.peekNext());
}

void MateriaPattern::poststep()
{
  m_progress = (m_progress + 1) % RING_COUNT;
  if (RING_COUNT - m_progress == RING_LAST) {
    m_colorset.getNext();
  }
}
