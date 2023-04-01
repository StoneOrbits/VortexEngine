#include "SolidPattern.h"

SolidPattern::SolidPattern(const PatternArgs &args) :
  BasicPattern(args),
  m_colIndex(0)
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
  m_colIndex = args.arg7;
}

void SolidPattern::getArgs(PatternArgs &args) const
{
  BasicPattern::getArgs(args);
  args.arg7 = m_colIndex;
  args.numArgs += 1;
}

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
