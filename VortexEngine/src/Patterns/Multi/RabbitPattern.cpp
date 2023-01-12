#include "RabbitPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../PatternBuilder.h"

RabbitPattern::RabbitPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t gapDuration1,
  uint8_t onDuration2, uint8_t offDuration2, uint8_t gapDuration2) :
  HybridPattern(),
  m_tipArgs(onDuration1, offDuration1, gapDuration1),
  m_topArgs(onDuration2, offDuration2, gapDuration2)
{
  m_patternID = PATTERN_RABBIT;
}

RabbitPattern::RabbitPattern(const PatternArgs &args) :
  RabbitPattern()
{
  setArgs(args);
}

RabbitPattern::~RabbitPattern()
{
}

// init the pattern to initial state
void RabbitPattern::init()
{
  HybridPattern::init();
  Colorset tipsColor(m_colorset.get(0));
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    if (isFingerTip(p)) {
      //tipsArgs = { onDuration, offDuration, gapDuration};
      setPatternAt(p, PatternBuilder::makeSingle(PATTERN_BASIC, &m_tipArgs), &tipsColor);
    } else {
      //topsArgs = { onDuration2, offDuration2, gapDuration2 };
      setPatternAt(p, PatternBuilder::makeSingle(PATTERN_BASIC, &m_topArgs));
    }
  }
}

// must override the serialize routine to save the pattern
void RabbitPattern::serialize(ByteStream& buffer) const
{
  HybridPattern::serialize(buffer);
  buffer.serialize(m_tipArgs.arg1);
  buffer.serialize(m_tipArgs.arg2);
  buffer.serialize(m_tipArgs.arg3);
  buffer.serialize(m_topArgs.arg1);
  buffer.serialize(m_topArgs.arg2);
  buffer.serialize(m_topArgs.arg3);
}

void RabbitPattern::unserialize(ByteStream& buffer)
{
  HybridPattern::unserialize(buffer);
  buffer.unserialize(&m_tipArgs.arg1);
  buffer.unserialize(&m_tipArgs.arg2);
  buffer.unserialize(&m_tipArgs.arg3);
  buffer.unserialize(&m_topArgs.arg1);
  buffer.unserialize(&m_topArgs.arg2);
  buffer.unserialize(&m_topArgs.arg3);
}

void RabbitPattern::setArgs(const PatternArgs& args)
{
  HybridPattern::setArgs(args);
  m_tipArgs.arg1 = args.arg1;
  m_tipArgs.arg2 = args.arg2;
  m_tipArgs.arg3 = args.arg3;
  m_topArgs.arg1 = args.arg4;
  m_topArgs.arg2 = args.arg5;
  m_topArgs.arg3 = args.arg6;
}

void RabbitPattern::getArgs(PatternArgs& args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_tipArgs.arg1;
  args.arg2 = m_tipArgs.arg2;
  args.arg3 = m_tipArgs.arg3;
  args.arg4 = m_topArgs.arg1;
  args.arg5 = m_topArgs.arg2;
  args.arg5 = m_topArgs.arg3;
  args.numArgs += 6;
}
