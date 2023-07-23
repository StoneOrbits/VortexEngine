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
  if (m_flip) {
    // do a flip as bender would say
    doFlip();
  } else {
    // if there is no flips then just do a normal blink
    doBlink();
  }
  // now if there is a flip amount set
  if (m_numFlips > 0) {
    // then increase the flip counter and modulate it
    m_flip = (m_flip + 1) % m_numFlips;
  }
}

void BlendPattern::transitionValue(uint8_t &current, const uint8_t next, bool hue)
{
  // if the values are equal then there's no work to do
  if (next == current) {
    return;
  }
  // otherwise we can just blend as normal in closest direction
  int diff = next - current;
  if (hue) {
    // This will compute the difference such that it considers the wrapping
    // around from 255 to 0 and vice versa, taking the shortest path.
    // The extra + 256 before the modulus operator % 256 is to ensure that the
    // value inside the parentheses is positive, because in C++ the % operator
    // gives a remainder that has the same sign as the dividend, and you want
    // to avoid getting a negative number there.
    // This will result in a diff in the range -128 <= diff < 128, and a
    // positive value means that the shortest way from m_cur.hue to m_next.hue
    // is to increase m_cur.hue, while a negative value means that the shortest
    // way is to decrease m_cur.hue.
    diff = (int)(((uint8_t)((diff + 128 + 256) % 256)) - 128);
  }
  // calculate the step in the right direction, default step size is 1
  int step = (diff > 0) ? 1 : -1;
  // this will effectively perform abs(diff) so that we can check if the
  // blendspeed is larger than the diff or not.
  diff *= step;
  // Only add the blendspeed if there is enough difference before the target 
  // otherwise we may overshoot then oscillate around the target
  if (diff > m_blendSpeed) {
    // a blend speed of 0 will be standard speed
    step += m_blendSpeed;
  }
  // step the value forward
  current += step;
  // apply extra processing for circular hue translation
  if (hue) {
    // wrap around the hue automatically, this handles for example if step is -1
    // and it subtracts past 0 to -1, then adding 256 will result in 255
    current = (current + 256) % 256;
  }
}

void BlendPattern::doBlink()
{
  // if the current hue has reached the next hue
  if (m_cur == m_next) {
    // get the next color
    m_next = m_colorset.getNext();
  }
  // transition each value of the current hsv to the next hsv, the 'hue' has a
  // special handling where 255 is beside 0 (circular transition)
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
