#include "BasicPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

BasicPattern::BasicPattern(const PatternArgs &args) :
  Pattern(args),
  m_onDuration(0),
  m_offDuration(0),
  m_gapDuration(0),
  m_groupSize(0),
  m_skipCols(0),
  m_repeatGroup(0),
  m_realGroupSize(0),
  m_groupCounter(0),
  m_repeatCounter(0),
  m_blinkTimer(),
  m_gapTimer(),
  m_inGap(false)
{
  m_patternID = PATTERN_BASIC;
  setArgs(args);
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::init()
{
  // run base pattern init logic
  Pattern::init();

  m_inGap = false;

  // don't start the gap timer till we're in a gap
  m_gapTimer.init(TIMER_1_ALARM, m_gapDuration);

  // start the blink timer now
  m_blinkTimer.init(TIMER_2_ALARMS | TIMER_START, m_onDuration, m_offDuration);

  if (!m_groupSize || m_groupSize > m_colorset.numColors()) {
    m_realGroupSize = m_colorset.numColors();
  } else {
    m_realGroupSize = m_groupSize;
  }
  m_groupCounter = 0;
  m_repeatCounter = m_repeatGroup;
}

void BasicPattern::play()
{
  if (m_inGap) {
    // check to see if the gap timer triggered to end the gap
    if (m_gapTimer.onEnd()) {
      endGap();
    }
    Leds::clearIndex(m_ledPos);
    return;
  }

  // check the alarm to toggle the light
  AlarmID id = m_blinkTimer.alarm();

  if (id == 0) {
    // when timer 0 starts it's time to blink on
    onBlinkOn();
  } else if (id == 1) {
    // when timer 1 starts it's time to blink off
    onBlinkOff();
  } else if (m_blinkTimer.curAlarm() == 1 && m_blinkTimer.onEnd() && m_colorset.onEnd()) {
    // trigger the gap in the pattern
    triggerGap();
  }
}

void BasicPattern::triggerGap()
{
  if (m_gapDuration > 0) {
    // next frame will be a gap
    m_gapTimer.restart(1);
    m_inGap = true;
  }
  m_groupCounter = 0;
}

void BasicPattern::endGap()
{
  // next frame will not be a gap
  m_blinkTimer.restart(1);
  m_inGap = false;
  // Here we perform logic for repeating groups
  if (m_repeatCounter > 0) {
    // the repeat counter starts at group size and counts down
    // each time an entire group has been displayed
    m_repeatCounter--;
    // to "repeat" we simply move the colorset back one group size
    m_colorset.skip(-(int32_t)m_realGroupSize);
    // nothing more to do
    return;
  }
  if (m_skipCols > 0) {
    m_colorset.skip(m_skipCols);
  }
  if (!m_repeatCounter) {
    m_repeatCounter = m_repeatGroup;
  }
}

void BasicPattern::onBlinkOn()
{
  // set the target led with the given color
  Leds::setIndex(LED_0, m_colorset.getNext());
  Leds::setIndex(LED_1, m_colorset.cur());
}

void BasicPattern::onBlinkOff()
{
  if (m_offDuration > 0) {
    // clear the target led if there is an off duration
    Leds::clearIndex(LED_0);
    Leds::clearIndex(LED_1);
  }
  // count a blink in the group
  m_groupCounter++;
  // check if the group has reached the intended size
  if (m_groupCounter >= m_realGroupSize) {
    triggerGap();
  }
}

void BasicPattern::setArgs(const PatternArgs &args)
{
  Pattern::setArgs(args);
  m_onDuration = args.arg1;
  m_offDuration = args.arg2;
  m_gapDuration = args.arg3;
  m_groupSize = args.arg4;
  m_skipCols = args.arg5;
  m_repeatGroup = args.arg6;
}

void BasicPattern::getArgs(PatternArgs &args) const
{
  Pattern::getArgs(args);
  args.arg1 = m_onDuration;
  args.arg2 = m_offDuration;
  args.arg3 = m_gapDuration;
  args.arg4 = m_groupSize;
  args.arg5 = m_skipCols;
  args.arg6 = m_repeatGroup;
  args.numArgs += 6;
}
