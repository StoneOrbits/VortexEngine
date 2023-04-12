#include "Pattern.h"

#include <Arduino.h>

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include "../VortexConfig.h"

Pattern::Pattern() :
  m_patternID(PATTERN_FIRST),
  m_patternFlags(0),
  m_colorset(),
  m_ledPos(LED_FIRST),
  m_numArgs(0),
  m_argList()
{
}

Pattern::Pattern(const PatternArgs &args) :
  Pattern()
{
}

Pattern::~Pattern()
{
}

void Pattern::bind(LedPos pos)
{
  m_ledPos = pos;
}

void Pattern::init()
{
  m_colorset.resetIndex();
}

// must override the serialize routine to save the pattern
void Pattern::serialize(ByteStream &buffer) const
{
  buffer.serialize((uint8_t)m_patternID);
  m_colorset.serialize(buffer);
  PatternArgs args;
  getArgs(args);
  args.serialize(buffer);
}

// must override unserialize to load patterns
void Pattern::unserialize(ByteStream &buffer)
{
  // don't unserialize the pattern ID because it is already
  // unserialized by the pattern builder to decide which pattern
  // to instantiate, instead only unserialize the colorset
  m_colorset.unserialize(buffer);
  PatternArgs args;
  args.unserialize(buffer);
  if (args.numArgs > 0) {
    setArgs(args);
  }
}

void Pattern::setArg(uint8_t index, uint8_t value)
{
  if (index >= m_numArgs) {
    return;
  }
  *((uint8_t *)this + m_argList[index]) = value;
}

uint8_t Pattern::getArg(uint8_t index) const
{
  if (index >= m_numArgs) {
    return 0;
  }
  return *((uint8_t *)this + m_argList[index]);
}

void Pattern::setArgs(const PatternArgs &args)
{
  for (uint32_t i = 0; i < m_numArgs; ++i) {
    *((uint8_t *)this + m_argList[i]) = args.args[i];
  }
}

void Pattern::getArgs(PatternArgs &args) const
{
  args.init();
  for (uint32_t i = 0; i < m_numArgs; ++i) {
    args.addArgs(*((uint8_t *)this + m_argList[i]));
  }
}

bool Pattern::equals(const Pattern *other)
{
  if (!other) {
    return false;
  }
  // compare pattern id
  if (m_patternID != other->getPatternID()) {
    return false;
  }
  // then colorset
  if (!m_colorset.equals(other->getColorset())) {
    return false;
  }
  // then compare the extra params
  PatternArgs myArgs;
  PatternArgs otherArgs;
  getArgs(myArgs);
  other->getArgs(otherArgs);
  if (myArgs != otherArgs) {
    return false;
  }
  return true;
}

// change the colorset
void Pattern::setColorset(const Colorset *set)
{
  if (!set) {
    clearColorset();
  } else {
    m_colorset = *set;
  }
}

void Pattern::clearColorset()
{
  m_colorset.clear();
}

#ifdef VORTEX_LIB
void Pattern::registerArg(const char *name, uint8_t argOffset)
#else
void Pattern::registerArg(uint8_t argOffset)
#endif
{
  if (m_numArgs >= MAX_PATTERN_ARGS) {
    ERROR_LOG("too many args");
    return;
  }
#ifdef VORTEX_LIB
  m_argNameList[m_numArgs] = name;
#endif
  m_argList[m_numArgs++] = argOffset;
}
