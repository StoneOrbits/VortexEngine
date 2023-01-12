#include "HueShiftPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

HueShiftPattern::HueShiftPattern(uint8_t onDuration, uint8_t offDuration) :
  MultiLedPattern(),
  m_blinkOnDuration(onDuration),
  m_blinkOffDuration(offDuration),
  m_blinkTimer(),
  m_cur(0),
  m_next(0)
{
  m_patternID = PATTERN_HUESHIFT;
}

HueShiftPattern::HueShiftPattern(const PatternArgs &args) :
  HueShiftPattern()
{
  setArgs(args);
}

HueShiftPattern::~HueShiftPattern()
{
}

// init the pattern to initial state
void HueShiftPattern::init()
{
  MultiLedPattern::init();
  m_cur = m_colorset.getNext();
  m_next = m_colorset.getNext();
  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(m_blinkOnDuration);
  m_blinkTimer.addAlarm(m_blinkOffDuration);
  // start the blink timer from the next frame
  m_blinkTimer.start();
}

// pure virtual must override the play function
void HueShiftPattern::play()
{
  switch (m_blinkTimer.alarm()) {
  case 0: // turn on the leds for given mapping
    break;
  case 1: // turn off the leds
    Leds::clearAll();
  case -1: // just break and still run post-step
    return;
  }

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
  HSVColor showColor = m_cur;
  // set the target led with the current HSV color
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    Leds::setIndex(pos, hsv_to_rgb_generic(showColor));
    showColor.hue = (showColor.hue + 5) % 256;
  }
}

// must override the serialize routine to save the pattern
void HueShiftPattern::serialize(ByteStream &buffer) const
{
  MultiLedPattern::serialize(buffer);
  buffer.serialize(m_blinkOnDuration);
  buffer.serialize(m_blinkOffDuration);
}

void HueShiftPattern::unserialize(ByteStream &buffer)
{
  MultiLedPattern::unserialize(buffer);
  buffer.unserialize(&m_blinkOnDuration);
  buffer.unserialize(&m_blinkOffDuration);
}

void HueShiftPattern::setArgs(const PatternArgs &args)
{
  MultiLedPattern::setArgs(args);
  m_blinkOnDuration = args.arg1;
  m_blinkOffDuration = args.arg2;
}

void HueShiftPattern::getArgs(PatternArgs &args) const
{
  MultiLedPattern::getArgs(args);
  args.arg1 = m_blinkOnDuration;
  args.arg2 = m_blinkOffDuration;
  args.numArgs += 2;
}

#if SAVE_TEMPLATE == 1
void HueShiftPattern::saveTemplate(int level) const
{
  MultiLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"Speed\": %d,", m_speed);
  IndentMsg(level + 1, "\"Scale\": %d,", m_scale);
}
#endif
