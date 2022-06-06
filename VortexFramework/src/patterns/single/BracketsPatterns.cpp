#include "BracketsPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

BracketsPattern::BracketsPattern(uint8_t bracketDuration, uint8_t midDuration, uint8_t gapDuration) :
  Pattern(),
  m_bracketDuration(bracketDuration),
  m_midDuration(midDuration),
  m_gapDuration(gapDuration)
{
}

BracketsPattern::~BracketsPattern()
{
}

void BracketsPattern::init()
{
  // run basic pattern init logic
  Pattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_bracketDuration);
  m_blinkTimer.addAlarm(m_midDuration);
  m_blinkTimer.addAlarm(m_bracketDuration);
  m_blinkTimer.addAlarm(m_gapDuration);

  // start the blink timer from the next frame
  m_blinkTimer.start();
}

void BracketsPattern::play()
{
  // check the alarm to toggle the light
  AlarmID id = m_blinkTimer.alarm();

  switch (id) {
  case 0: // first bracket ended
    // begin mid
    Leds::setIndex(m_ledPos, m_colorset.getNext());
    break;
  case 1: // mid ended
    // begin second bracket
    Leds::setIndex(m_ledPos, m_colorset.getPrev());
    break;
  case 2: // second bracket ended
    // begin gap
    Leds::clearIndex(m_ledPos);
    break;
  case 3: // gap ended
    // begin first bracket
    Leds::setIndex(m_ledPos, m_colorset.getNext());
    break;
  }
}
