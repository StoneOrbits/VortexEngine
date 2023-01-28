#include "DripMorphPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

DripMorphPattern::DripMorphPattern(uint8_t blinkOn, uint8_t blinkOff, uint8_t speed) :
  MultiLedPattern(),
  m_blinkOnDuration(blinkOn),
  m_blinkOffDuration(blinkOff),
  m_blinkTimer(),
  m_speed(speed),
  m_cur(),
  m_next()
{
  m_patternID = PATTERN_DRIPMORPH;
}

DripMorphPattern::DripMorphPattern(const PatternArgs &args) :
  DripMorphPattern()
{
  setArgs(args);
}

DripMorphPattern::~DripMorphPattern()
{
}

void DripMorphPattern::init()
{
  MultiLedPattern::init();
  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(m_blinkOnDuration);
  m_blinkTimer.addAlarm(m_blinkOffDuration);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // convert current/next colors to HSV
  m_cur = m_colorset.getNext();
  m_next = m_colorset.getNext();
}

// pure virtual must override the play function
void DripMorphPattern::play()
{
  switch (m_blinkTimer.alarm()) {
  case -1: // just break
    break;
  case 0: // turn on the leds for given mapping
    blinkOn();
    break;
  case 1: // turn off the leds
    blinkOff();
    break;
  }
}

void DripMorphPattern::setArgs(const PatternArgs &args)
{
  MultiLedPattern::setArgs(args);
  m_blinkOnDuration = args.arg1;
  m_blinkOffDuration = args.arg2;
  m_speed = args.arg3;
}

void DripMorphPattern::getArgs(PatternArgs &args) const
{
  MultiLedPattern::getArgs(args);
  args.arg1 = m_blinkOnDuration;
  args.arg2 = m_blinkOffDuration;
  args.arg3 = m_speed;
  args.numArgs += 3;
}

#if SAVE_TEMPLATE == 1
void DripMorphPattern::saveTemplate(int level) const
{
  MultiLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"BlinkOnDuration\": %d,", m_blinkOnDuration);
  IndentMsg(level + 1, "\"BlinkOffDuration\": %d,", m_blinkOffDuration);
  IndentMsg(level + 1, "\"Speed\": %d,", m_speed);
}
#endif

void DripMorphPattern::blinkOn()
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
  // set the target led with the current HSV color
  Leds::setAllEvens(m_cur);
  Leds::setAllOdds(m_colorset.cur());
}

void DripMorphPattern::blinkOff()
{
  Leds::clearAll();
}
