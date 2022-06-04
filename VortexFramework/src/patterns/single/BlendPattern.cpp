#include "BlendPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

#include <math.h>

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
  // if the current hue has reached the next hue
  if (m_cur.hue == m_next.hue) {
    // copy over the sat/val
    m_cur = m_next;
    // get the next color and convert to hsv
    m_next = m_colorset.getNext();
  }
  // check which direction is closer for the next hue
  int sign = (abs(m_next.hue - m_cur.hue) < 128) ? 1 : -1;
  // move hue in the direction of next hue at chosen speed
  // NOTE: if the speed isn't a multiple of the hue values then
  //       it will cause oscillation around the target hue 
  //       because it will never reach the target hue and 
  //       always over/under shoot
  m_cur.hue += m_speed * sign;
  // set the target led with the current HSV color
  Leds::setIndex(m_ledPos, m_cur);
}