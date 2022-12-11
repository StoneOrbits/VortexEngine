#include "SnowballPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define WORM_SIZE 6

SnowballPattern::SnowballPattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_progress()
{
  m_patternID = PATTERN_SNOWBALL;
}

SnowballPattern::SnowballPattern(const PatternArgs &args) :
  SnowballPattern(args.arg1, args.arg2, args.arg3)
{
}

SnowballPattern::~SnowballPattern()
{
}

// init the pattern to initial state
void SnowballPattern::init()
{
  BlinkStepPattern::init();

  // start at index 1
  m_colorset.setCurIndex(1);
}

void SnowballPattern::blinkOn()
{
  Leds::setAll(m_colorset.get(0));
  for (int body = 0; body < WORM_SIZE; ++body) {
    if (body + m_progress < LED_COUNT) {
      Leds::setIndex((LedPos)(body + m_progress), m_colorset.cur());
    } else {
      RGBColor col = m_colorset.peekNext();
      if (m_colorset.curIndex() == m_colorset.numColors() - 1) {
        col = m_colorset.peek(2);
      }
      Leds::setIndex((LedPos)((body + m_progress) % LED_COUNT), col);
    }
  }
}

void SnowballPattern::poststep()
{
  m_colorset.skip();
  if (m_colorset.curIndex() == 0) {
    m_progress = (m_progress + 1) % LED_COUNT;
    m_colorset.skip();
  }
}
