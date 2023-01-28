#include "SolidPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Log/Log.h"

SolidPattern::SolidPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration, uint8_t colIndex) :
  BasicPattern(onDuration, offDuration, gapDuration),
  m_colIndex(colIndex)
{
  m_patternID = PATTERN_SOLID;
}

SolidPattern::SolidPattern(const PatternArgs &args) :
  SolidPattern()
{
  setArgs(args);
}

SolidPattern::~SolidPattern()
{
}

void SolidPattern::init()
{
  BasicPattern::init();
}

void SolidPattern::play()
{
  BasicPattern::play();
}

void SolidPattern::setArgs(const PatternArgs &args)
{
  BasicPattern::setArgs(args);
  m_colIndex = args.arg4;
}

void SolidPattern::getArgs(PatternArgs &args) const
{
  BasicPattern::getArgs(args);
  args.arg4 = m_colIndex;
  args.numArgs += 1;
}

#if SAVE_TEMPLATE == 1
void SolidPattern::saveTemplate(int level) const
{
  BasicPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"ColorIndex\": %d,", m_colIndex);
}
#endif

// callbacks for blinking on/off, can be overridden by derived classes
void SolidPattern::onBlinkOn()
{
  if (m_colIndex == 0) {
    m_colorset.resetIndex();
  } else {
    m_colorset.setCurIndex(m_colIndex - 1);
  }
  BasicPattern::onBlinkOn();
}
