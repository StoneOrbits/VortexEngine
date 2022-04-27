#include "Pattern.h"

#include <Arduino.h>

Pattern::Pattern() :
  m_patternID(PATTERN_STROBE)
{
}

Pattern::~Pattern()
{
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

