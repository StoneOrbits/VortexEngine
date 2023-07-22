#include "BlendPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

#include <math.h>

BlendPattern::BlendPattern(const PatternArgs &args) :
  BasicPattern(args),
  m_blendSpeed(1),
  m_numFlips(1),
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
  if (!m_flip) {
    // if there is no flips then just do a normal blink
    doBlink();
  } else {
    // otherwise do a flip as bender would say
    doFlip();
  }
  // now if there is a flip amount set
  if (m_numFlips > 0) {
    // then increase the flip counter and modulate it
    m_flip = (m_flip + 1) % m_numFlips;
  }
}

void BlendPattern::transitionValue(uint8_t &current, const uint8_t next, bool hue)
{
  if (next == current) {
    return;
  }
  int diff;
  if (hue) {
    // this wild thing calculates closest circular direction on the hue circle
    diff = (int)(((uint8_t)((next - current + 128 + 256) % 256)) - 128);
  } else {
    // otherwise we can just blend as normal in closest direction
    diff = next - current;
  }
  int sign = (diff > 0) ? 1 : -1;
  diff *= sign; // this will effectively perform abs(diff)
  if (m_blendSpeed && (diff >= m_blendSpeed || -diff >= m_blendSpeed)) {
    sign *= m_blendSpeed;
  }
  if (hue) {
    // wrap around the hue automatically
    current = (current + sign + 256) % 256;
  } else {
    // otherwise just add the value
    current += sign;
  }
}

void BlendPattern::doBlink()
{
  // if the current hue has reached the next hue
  if (m_cur == m_next) {
    // get the next color
    m_next = m_colorset.getNext();
  }
  transitionValue(m_cur.hue, m_next.hue, true);
  transitionValue(m_cur.sat, m_next.sat, false);
  transitionValue(m_cur.val, m_next.val, false);
  // set the target led with the current HSV color
  Leds::setIndex(m_ledPos, hsv_to_rgb_generic(m_cur));
}

void BlendPattern::doFlip()
{
  uint32_t hueOffset = m_flip * (255 / m_numFlips);
  // generate an inverse hue based on the current hue position
  HSVColor hsvCol((m_cur.hue + hueOffset) % 256, m_cur.sat, m_cur.val);
  // convert the HSV to RGB with the generic function because
  // this will generate a different appearance from using the
  // default hsv_to_rgb_rainbow()
  Leds::setIndex(m_ledPos, hsv_to_rgb_generic(hsvCol));
}
