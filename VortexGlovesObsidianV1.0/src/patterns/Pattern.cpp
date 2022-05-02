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
  if (set) {
    set->init();
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
