#include "Pattern.h"

#include <Arduino.h>

#include "../TimeControl.h"
#include "../Colorset.h"
#include "../Log.h"

Pattern::Pattern() :
  m_patternID(PATTERN_STROBE),
  m_pColorset(nullptr),
  m_ledPos(LED_FIRST)
{
}

Pattern::~Pattern()
{
}

void Pattern::init(Colorset *set, LedPos pos)
{
  m_pColorset = set;
  m_ledPos = pos;
  // if the led position is LED_COUNT that means this pattern is
  // a complex pattern that operates on all LEDS. If it's not a
  // complex pattern then the pattern needs to be fast-forwarded
  // based on the finger-tick-offset
  if (pos != LED_COUNT) {
    // skip forward however many ticks this led is offset
    skip(Time::getTickOffset(pos));
  }
}

void Pattern::skip(uint32_t ticks)
{
  uint32_t startTime = Time::startSimulation();
  for (int i = 0; i < ticks; ++i) {
    play();  // simulate playing the pattern
    Time::tickSimulation();
  }
  uint32_t endTime = Time::endSimulation();
}

// must override the serialize routine to save the pattern
void Pattern::serialize() const
{
  Serial.print(m_patternID);
}

// must override unserialize to load patterns
void Pattern::unserialize()
{
}
