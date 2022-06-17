#include "BracketsPattern.h"

#include "../../Colorset.h"
#include "../../Leds.h"

BracketsPattern::BracketsPattern(uint8_t bracketDuration, uint8_t midDuration, uint8_t offDuration) :
  SingleLedPattern(),
  m_bracketDuration(bracketDuration),
  m_midDuration(midDuration),
  m_offDuration(offDuration)
{
}

BracketsPattern::~BracketsPattern()
{
}

void BracketsPattern::init()
{
  // run basic pattern init logic
  SingleLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_bracketDuration);
  m_blinkTimer.addAlarm(m_midDuration);
  m_blinkTimer.addAlarm(m_bracketDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the next frame
  m_blinkTimer.start();
}

void BracketsPattern::play()
{
  // check the alarm to toggle the light
  switch (m_blinkTimer.alarm()) {
  case 0: // gap ended
    // begin first bracket
    Leds::setIndex(m_ledPos, m_colorset.getNext());
    break;
  case 1: // first bracket ended
    // begin mid
    Leds::setIndex(m_ledPos, m_colorset.getNext());
    break;
  case 2: // mid ended
    // begin second bracket
    Leds::setIndex(m_ledPos, m_colorset.getPrev());
    break;
  case 3: // second bracket ended
    // begin gap
    Leds::clearIndex(m_ledPos);
    break;
  }
}
