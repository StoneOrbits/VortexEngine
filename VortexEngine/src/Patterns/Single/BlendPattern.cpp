#include "BlendPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

#include <math.h>

BlendPattern::BlendPattern(const PatternArgs &args) :
  BasicPattern(args),
  m_hueOffset(0),
  m_numFlips(1),
  m_cur(),
  m_next(),
  m_flip(0)
{
  m_patternID = PATTERN_BLEND;
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

void BlendPattern::setArgs(const PatternArgs &args)
{
  BasicPattern::setArgs(args);
  m_hueOffset = args.arg7;
  m_numFlips = args.arg8;
}

void BlendPattern::getArgs(PatternArgs &args) const
{
  BasicPattern::getArgs(args);
  args.arg7 = m_hueOffset;
  args.arg8 = m_numFlips;
  args.numArgs += 2;
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

void BlendPattern::doBlink()
{
  // if the current hue has reached the next hue
  if (m_cur.hue == m_next.hue) {
    // copy over the sat/val
    m_cur = m_next;
    // get the next color and convert to hsv
    m_next = m_colorset.getNext();
  }
  // check which direction is closer for the next hue
  // tbh idk really how this even works anymore but it does
  int sign = ((((m_next.hue + 255) - m_cur.hue) % 256) <= 128) ? 1 : -1;
  // move hue in the direction of next hue at chosen speed
  // NOTE: if the speed isn't a multiple of the hue values then
  //       it will cause oscillation around the target hue
  //       because it will never reach the target hue and
  //       always over/under shoot
  m_cur.hue += sign;
  // apply the hue offset
  m_cur.hue += m_hueOffset;
  HSVColor showColor = m_cur;
  // set the target led with the current HSV color
  Leds::setIndex(m_ledPos, hsv_to_rgb_generic(showColor));
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
