#include "BlendPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <math.h>

BlendPattern::BlendPattern(uint8_t onDuration, uint8_t offDuration, uint8_t blendSpeed) :
  BasicPattern(onDuration, offDuration),
  m_speed(blendSpeed),
  m_cur(),
  m_next()
{
  m_patternID = PATTERN_BLEND;
}

BlendPattern::BlendPattern(const PatternArgs &args) :
  BlendPattern(args.arg1, args.arg2, args.arg3)
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

void BlendPattern::serialize(ByteStream &buffer) const
{
  BasicPattern::serialize(buffer);
  buffer.serialize(m_speed);
}

void BlendPattern::unserialize(ByteStream &buffer)
{
  BasicPattern::unserialize(buffer);
  buffer.unserialize(&m_speed);
}

#if SAVE_TEMPLATE == 1
void BlendPattern::saveTemplate(int level) const
{
  BasicPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"BlendSpeed\": %d,", m_speed);
}
#endif

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
  // tbh idk really how this even works anymore but it does
  int sign = ((((m_next.hue + 255) - m_cur.hue) % 256) <= 128) ? 1 : -1;
  // move hue in the direction of next hue at chosen speed
  // NOTE: if the speed isn't a multiple of the hue values then
  //       it will cause oscillation around the target hue 
  //       because it will never reach the target hue and 
  //       always over/under shoot
  m_cur.hue += m_speed * sign;
  HSVColor showColor = m_cur;
  // set the target led with the current HSV color
  Leds::setIndex(m_ledPos, hsv_to_rgb_generic(showColor));
}
