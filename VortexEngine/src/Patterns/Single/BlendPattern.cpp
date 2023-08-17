#include "BlendPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

BlendPattern::BlendPattern(const PatternArgs &args) :
  BasicPattern(args),
  m_blendSpeed(0),
  m_numFlips(0),
  m_hueShift(0),
  m_curHSV(),
  m_nextHSV(),
  m_curRGB(),
  m_nextRGB(),
  m_flip(0)
{
  m_patternID = PATTERN_BLEND;
  REGISTER_ARG(m_blendSpeed);
  REGISTER_ARG(m_numFlips);
  REGISTER_ARG(m_hueShift);
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
  m_curHSV = m_curRGB = m_colorset.getNext();
  m_nextHSV = m_nextRGB = m_colorset.getNext();
  // reset the flip count
  m_flip = 0;
}

void BlendPattern::onBlinkOn()
{
  bool timeToTick = ((Time::getCurtime() % 4) == 0);
  if (timeToTick) {
    if (m_hueShift) {
      if (m_curHSV == m_nextHSV) {
        m_nextHSV = m_colorset.getNext();
      }
      interpolate(m_curHSV.hue, m_nextHSV.hue, INTERP_WRAP);
      interpolate(m_curHSV.sat, m_nextHSV.sat);
      interpolate(m_curHSV.val, m_nextHSV.val);
    } else {
      if (m_curRGB == m_nextRGB) {
        m_nextRGB = m_colorset.getNext();
      }
      interpolate(m_curRGB.red, m_nextRGB.red);
      interpolate(m_curRGB.green, m_nextRGB.green);
      interpolate(m_curRGB.blue, m_nextRGB.blue);
    }
  }

  if (!m_flip) {
    Leds::setIndex(m_ledPos, (m_hueShift ? (RGBColor)m_curHSV : m_curRGB));
  } else {
    HSVColor hsvCol = (m_hueShift ? m_curHSV : rgb_to_hsv_generic(m_curRGB));
    hsvCol.hue += (m_flip * (255 / m_numFlips));
    Leds::setIndex(m_ledPos, hsv_to_rgb_generic(hsvCol));
  }
  m_flip++;
  if (m_flip >= m_numFlips) {
    m_flip = 0;
  }
}

void BlendPattern::interpolate(uint8_t &current, const uint8_t next, InterpWrapType wrap)
{
  if (next == current) {
    return;
  }
  // calculate the difference between the current and next value, this will calculate
  // either the circular difference if INTERP_WRAP is set, or just the plain integer
  // difference if it's not set. The circular difference accounts for wrapping past
  // 255 <-> 0 to get to the next value quicker than the other direction
  int diff = next - current;
  if (wrap == INTERP_WRAP) {
    // wrap around from 256 to 0 and interpolate through 0 if it's a closer path
    diff = (int)(((uint8_t)((diff + 128 + 256) % 256)) - 128);
  }
  // the step will either be -1 or 1 based on the direction of the difference
  int step = (diff > 0) ? 1 : -1;
  // multiply the difference by the step, this effectively does diff = abs(diff)
  diff *= step;
  // now that we know diff is absolute (positive) we can compare it to the blend speed
  if (diff > m_blendSpeed) {
    // if the diff is large enough to fit the blendspeed then include it in the step
    step += (step * m_blendSpeed);
  }
  // add the step to the current value to move towards the next value
  current += step;
}