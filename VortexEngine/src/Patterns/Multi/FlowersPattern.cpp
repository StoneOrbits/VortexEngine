#include "FlowersPattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"

FlowersPattern::FlowersPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t gapDuration1,
  uint8_t onDuration2, uint8_t offDuration2, uint8_t gapDuration2) :
  HybridPattern(),
  m_firstPatternArgs(onDuration1, offDuration1, gapDuration1),
  m_secondPatternArgs(onDuration2, offDuration2, gapDuration2)
{
  m_patternID = PATTERN_FLOWERS;
}

FlowersPattern::FlowersPattern(const PatternArgs &args) :
  FlowersPattern()
{
  setArgs(args);
}

FlowersPattern::~FlowersPattern()
{
}

// init the pattern to initial state
void FlowersPattern::init()
{
  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init();
  
  // split colorset
  Colorset sideSet(m_colorset.get(0), m_colorset.get(1));
  Colorset midSet(m_colorset.get(2), m_colorset.get(3));

  // thumb and pinkie use duration 1
  setPatternAt(THUMB_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(THUMB_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(PINKIE_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);
  setPatternAt(PINKIE_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &sideSet);

  // middle index ring use duration 2
  setPatternAt(MIDDLE_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(MIDDLE_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(INDEX_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(INDEX_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(RING_TOP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
  setPatternAt(RING_TIP, PatternBuilder::makeSingle(PATTERN_BASIC, &m_firstPatternArgs), &midSet);
}

// must override the serialize routine to save the pattern
void FlowersPattern::serialize(ByteStream& buffer) const
{
  HybridPattern::serialize(buffer);
  buffer.serialize(m_firstPatternArgs.arg1);
  buffer.serialize(m_firstPatternArgs.arg2);
  buffer.serialize(m_firstPatternArgs.arg3);
  buffer.serialize(m_secondPatternArgs.arg1);
  buffer.serialize(m_secondPatternArgs.arg2);
  buffer.serialize(m_secondPatternArgs.arg3);
}

void FlowersPattern::unserialize(ByteStream& buffer)
{
  HybridPattern::unserialize(buffer);
  buffer.unserialize(&m_firstPatternArgs.arg1);
  buffer.unserialize(&m_firstPatternArgs.arg2);
  buffer.unserialize(&m_firstPatternArgs.arg3);
  buffer.unserialize(&m_secondPatternArgs.arg1);
  buffer.unserialize(&m_secondPatternArgs.arg2);
  buffer.unserialize(&m_secondPatternArgs.arg3);
}

void FlowersPattern::setArgs(const PatternArgs& args)
{
  HybridPattern::setArgs(args);
  m_firstPatternArgs.arg1 = args.arg1;
  m_firstPatternArgs.arg2 = args.arg2;
  m_firstPatternArgs.arg3 = args.arg3;
  m_secondPatternArgs.arg1 = args.arg4;
  m_secondPatternArgs.arg2 = args.arg5;
  m_secondPatternArgs.arg3 = args.arg6;
}

void FlowersPattern::getArgs(PatternArgs& args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_firstPatternArgs.arg1;
  args.arg2 = m_firstPatternArgs.arg2;
  args.arg3 = m_firstPatternArgs.arg3;
  args.arg4 = m_secondPatternArgs.arg1;
  args.arg5 = m_secondPatternArgs.arg2;
  args.arg6 = m_secondPatternArgs.arg3;
  args.numArgs += 6;
}
