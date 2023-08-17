#include "BlendPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

BlendPattern::BlendPattern(const PatternArgs &args) :
  BasicPattern(args),
  m_blendSpeed(0),
  m_numFlips(0),
  m_cur(),
  m_next(),
  m_flip(0)
{
  m_patternID = PATTERN_BLEND;
  REGISTER_ARG(m_blendSpeed);
  REGISTER_ARG(m_numFlips);
  setArgs(args);
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
  // reset the flip count
  m_flip = 0;
}

void BlendPattern::onBlinkOn()
{
  // if the current hue has reached the next hue
  if (m_cur == m_next) {
    // get the next color
    m_next = m_colorset.getNext();
  }
  // only transition the shift once every 4 ticks, this will make sure the shift doesn't go
  // to fast and it allows the shiftspeed parameter to speed it up to preference
  if ((Time::getCurtime() % 4) == 0) {
    // transition each value of the current hsv to the next hsv, the 'hue' has a
    // special handling where 255 is beside 0 (circular transition)
    m_cur.red = interpolate(m_cur.red, m_next.red);
    m_cur.green = interpolate(m_cur.green, m_next.green);
    m_cur.blue = interpolate(m_cur.blue, m_next.blue);
  }
  if (!m_flip) {
    Leds::setIndex(m_ledPos, m_cur);
    return;
  }
  // make a copy of the current color being rendered so that it can be
  // flipped to an inverse color if the flips are enabled
  HSVColor hsvCol = m_cur;
  // note: no division by zero because m_numFlips cannot be 0 if m_flip is non-zero
  hsvCol.hue += (m_flip * (255 / m_numFlips));
  // convert the HSV to RGB with the generic function because
  // this will generate a different appearance from using the
  // default hsv_to_rgb_rainbow()
  Leds::setIndex(m_ledPos, hsv_to_rgb_generic(hsvCol));
  // increase the flip counter and modulate it, this actually takes less space
  // on avr than using a modulo of numflips, because you must check for nonzero
  m_flip++;
  if (m_flip >= m_numFlips) {
    m_flip = 0;
  }
}

uint8_t BlendPattern::interpolate(uint16_t current, uint16_t next)
{
  if (current < next) {
    if ((next - current) > m_blendSpeed) {
      return current + m_blendSpeed;
    }
    return current + 1;
  } else if (current > next) {
    if ((current - next) > m_blendSpeed) {
      return current - m_blendSpeed;
    }
    return current - 1;
  }
  return next;
}
