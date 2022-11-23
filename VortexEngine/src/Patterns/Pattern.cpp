#include "Pattern.h"

#include <Arduino.h>

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Log/Log.h"

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
  // call init here? idk
  //init();
}

void Pattern::init()
{
  m_colorset.resetIndex();
}

// must override the serialize routine to save the pattern
void Pattern::serialize(ByteStream &buffer) const
{
  //DEBUG_LOG("Serialize");
  buffer.serialize((uint8_t)m_patternID);
  m_colorset.serialize(buffer);
}

// must override unserialize to load patterns
void Pattern::unserialize(ByteStream &buffer)
{
  //DEBUG_LOG("Unserialize");
  // don't unserialize the pattern ID because it is already
  // unserialized by the pattern builder to decide which pattern
  // to instantiate, instead only unserialize the colorset
  m_colorset.unserialize(buffer);
}

#ifdef TEST_FRAMEWORK
void Pattern::saveTemplate(int level) const
{
  IndentMsg(level, "\"PatternID\": %d,", m_patternID);
  IndentMsg(level, "\"Colorset\": {");
  m_colorset.saveTemplate(level + 1);
  IndentMsg(level, "},");
  IndentMsg(level, "\"Params\": {");
  // derived classes will print out their params, 
  // then the caller will close the block
}
#endif

bool Pattern::equals(const Pattern *other)
{
  if (!other) {
    return false;
  }
  // and pattern id
  if (m_patternID != other->getPatternID()) {
    return false;
  }
  // only compare colorset
  if (!m_colorset.equals(other->getColorset())) {
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
