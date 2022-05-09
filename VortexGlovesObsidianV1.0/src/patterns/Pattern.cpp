#include "Pattern.h"

#include <Arduino.h>

#include "../TimeControl.h"
#include "../Colorset.h"
#include "../Log.h"

Pattern::Pattern() :
  m_patternID(PATTERN_NONE),
  m_pColorset(nullptr),
  m_ledPos(LED_FIRST),
  m_patternStartTick(0)
{
}

Pattern::~Pattern()
{
}

void Pattern::init(Colorset *set, LedPos pos)
{
  m_patternStartTick = Time::getCurtime(m_ledPos);
  m_pColorset = set;
  m_ledPos = pos;
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

uint32_t Pattern::getPatternTick() const
{
  return Time::getCurtime(m_ledPos) - m_patternStartTick;
}
