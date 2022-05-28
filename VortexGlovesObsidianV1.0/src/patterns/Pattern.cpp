#include "Pattern.h"

#include <Arduino.h>

#include "../SerialBuffer.h"
#include "../TimeControl.h"
#include "../Colorset.h"
#include "../Log.h"

Pattern::Pattern() :
  m_patternID(PATTERN_FIRST),
  m_patternFlags(0),
  m_colorset(),
  m_ledPos(LED_FIRST)
{
}

Pattern::~Pattern()
{
}

void Pattern::bind(const Colorset *set, LedPos pos)
{
  if (!set) {
    m_colorset.clear();
  } else {
    m_colorset = *set;
  }
  m_ledPos = pos;
  init();
}

void Pattern::init()
{
}

// must override the serialize routine to save the pattern
void Pattern::serialize(SerialBuffer &buffer) const
{
  DEBUG("Serialize");
  buffer.serialize((uint8_t)m_patternID);
  m_colorset.serialize(buffer);
}

// must override unserialize to load patterns
void Pattern::unserialize(SerialBuffer &buffer)
{
  DEBUG("Unserialize");
  // don't unserialize the pattern ID because it is already
  // unserialized by the pattern builder to decide which pattern
  // to instantiate, instead only unserialize the colorset
  m_colorset.unserialize(buffer);
}

bool Pattern::equals(const Pattern *other)
{
  if (!other) {
    return false;
  }
  // only compare colorset
  if (!m_colorset.equals(other->getColorset())) {
    return false;
  }
  // and pattern id
  if (m_patternID != other->getPatternID()) {
    return false;
  }
  return true;
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
