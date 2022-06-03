#include "BlendPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

BlendPattern::BlendPattern(uint8_t onDuration, uint8_t offDuration, uint8_t blendSpeed) :
  BasicPattern(onDuration, offDuration),
  m_speed(blendSpeed),
  m_cur(),
  m_next()
{
}

BlendPattern::~BlendPattern()
{
}

void BlendPattern::init()
{
  // run basic pattern init logic
  BasicPattern::init();
  // convert current/next colors to HSV
  m_cur = m_colorset.getNext();
  m_next = m_colorset.getNext();
}

void BlendPattern::play()
{
  BasicPattern::play();
}

void BlendPattern::onBlinkOn()
{
  int32_t hueDiff = 0;
  // if the current hue has reached the next hue
  if (m_cur.hue == m_next.hue) {
    // copy over the sat/val
    m_cur = m_next;
    // get the next color and convert to hsv
    m_next = m_colorset.getNext();
  }
  // if the next color is greater than current in hue
  if (m_next.hue > m_cur.hue) {
    // increment current hue
    m_cur.hue += m_speed;
  } else {
    // otherwise decrement current hue
    m_cur.hue -= m_speed;
  }
  // set the target led with the current HSV color
  Leds::setIndex(m_ledPos, m_cur);
}