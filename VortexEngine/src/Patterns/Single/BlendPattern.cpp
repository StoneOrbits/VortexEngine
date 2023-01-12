#include "BlendPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <math.h>

BlendPattern::BlendPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration, uint8_t offset) :
  BasicPattern(onDuration, offDuration, gapDuration),
  m_hueOffset(offset),
  m_cur(),
  m_next()
{
  m_patternID = PATTERN_BLEND;
}

BlendPattern::BlendPattern(const PatternArgs &args) :
  BlendPattern()
{
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
}

void BlendPattern::play()
{
  BasicPattern::play();
}

void BlendPattern::serialize(ByteStream &buffer) const
{
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_onDuration);
  buffer.serialize(m_offDuration);
  buffer.serialize(m_gapDuration);
  buffer.serialize(m_hueOffset);
}

void BlendPattern::unserialize(ByteStream &buffer)
{
  SingleLedPattern::unserialize(buffer);
  buffer.unserialize(&m_onDuration);
  buffer.unserialize(&m_offDuration);
  buffer.unserialize(&m_gapDuration);
  buffer.unserialize(&m_hueOffset);
}

void BlendPattern::setArgs(const PatternArgs &args)
{
  SingleLedPattern::setArgs(args);
  m_onDuration = args.arg1;
  m_offDuration = args.arg2;
  m_gapDuration = args.arg3;
  m_hueOffset = args.arg4;
}

void BlendPattern::getArgs(PatternArgs &args) const
{
  SingleLedPattern::getArgs(args);
  args.arg1 = m_onDuration;
  args.arg2 = m_offDuration;
  args.arg3 = m_gapDuration;
  args.arg4 = m_hueOffset;
  args.numArgs += 4;
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
  m_cur.hue += sign;
  // apply the hue offset
  m_cur.hue += m_hueOffset * m_ledPos;
  HSVColor showColor = m_cur;
  // set the target led with the current HSV color
  Leds::setIndex(m_ledPos, hsv_to_rgb_generic(showColor));
}
