#include "SolidPattern.h"

#include "../../SerialBuffer.h"

SolidPattern::SolidPattern(uint8_t colIndex, uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration) :
  BasicPattern(onDuration, offDuration, gapDuration),
  m_colIndex(colIndex)
{
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

void SolidPattern::serialize(SerialBuffer &buffer) const
{
  BasicPattern::serialize(buffer);
  buffer.serialize(m_colIndex);
}

void SolidPattern::unserialize(SerialBuffer &buffer)
{
  BasicPattern::unserialize(buffer);
  buffer.unserialize(&m_colIndex);
}

// callbacks for blinking on/off, can be overridden by derived classes
void SolidPattern::onBlinkOn()
{
  if (m_colIndex == 0) {
    m_colorset.init();
  } else {
    m_colorset.setCurIndex(m_colIndex - 1);
  }
  BasicPattern::onBlinkOn();
}
