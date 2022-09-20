#include "WarpWormPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"
#include "../../Timings.h"

WarpWormPattern::WarpWormPattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  BlinkStepPattern(DOPISH_ON_DURATION, DOPISH_OFF_DURATION, 100),
  m_progress()
{
}

WarpWormPattern::~WarpWormPattern()
{
}

// init the pattern to initial state
void WarpWormPattern::init()
{
  BlinkStepPattern::init();
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(1);
}

void WarpWormPattern::blinkOn()
{
  int wormSize = 6;
  Leds::setAll(m_colorset.get(0));
  for (int body = 0; body < wormSize; ++body) {
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

void WarpWormPattern::poststep()
{
  m_progress = (m_progress + 1) % LED_COUNT;
  if (m_progress == 0) {
    m_colorset.getNext();
    if (m_colorset.curIndex() == 0) {
      m_colorset.getNext();
    }
  }
}