#include "FillPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

FillPattern::FillPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_progress()
{
  m_patternID = PATTERN_FILL;
}

FillPattern::~FillPattern()
{
}

void FillPattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void FillPattern::blinkOn()
{
  Leds::setFingers(FINGER_FIRST, (Finger)m_progress, m_colorset.peekNext());
  Leds::setFingers((Finger)m_progress, FINGER_COUNT, m_colorset.cur());
}

void FillPattern::poststep()
{
  m_progress = (m_progress + 1) % FINGER_COUNT;
  if (m_progress == 0) {
    m_colorset.getNext();
  }
}
