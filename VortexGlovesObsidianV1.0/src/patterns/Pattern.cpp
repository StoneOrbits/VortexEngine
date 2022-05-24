#include "Pattern.h"

#include <Arduino.h>

#include "../SerialBuffer.h"
#include "../TimeControl.h"
#include "../Colorset.h"
#include "../Log.h"

Pattern::Pattern() :
  m_patternID(PATTERN_FIRST),
  m_colorset(),
  m_ledPos(LED_FIRST),
  m_patternFlags(0)
{
}

Pattern::~Pattern()
{
}

void Pattern::bind(const Colorset *set, LedPos pos)
{
  m_colorset = *set;
  m_ledPos = pos;
  init();
}

void Pattern::init()
{
}

// must override the serialize routine to save the pattern
void Pattern::serialize(SerialBuffer &buffer) const
{
  buffer.serialize(m_patternID);
}

// must override unserialize to load patterns
void Pattern::unserialize(SerialBuffer &buffer)
{
}

// change the colorset
void Pattern::setColorset(const Colorset *set)
{
  m_colorset = *set;
}

void Pattern::clearColorset()
{
  m_colorset.clear();
}
