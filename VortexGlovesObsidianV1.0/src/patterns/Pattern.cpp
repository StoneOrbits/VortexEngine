#include "Pattern.h"

#include <Arduino.h>

Pattern::Pattern() :
  m_patternID(PATTERN_STROBE)
{
}

Pattern::~Pattern()
{
}

void Pattern::init(Colorset *set, LedPos pos)
{
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
