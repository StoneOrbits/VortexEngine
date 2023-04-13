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
  m_reflectIndex(0),
  m_repeatGroup(0),
  m_realGroupSize(0),
  m_groupCounter(0),
  m_repeatCounter(0),
  m_reflect(false),
  m_blinkTimer(),
  m_gapTimer(),
  m_inGap(false)
{
  m_patternID = PATTERN_BASIC;
  REGISTER_ARG(m_onDuration);
  REGISTER_ARG(m_offDuration);
  REGISTER_ARG(m_gapDuration);
  REGISTER_ARG(m_groupSize);
  REGISTER_ARG(m_reflectIndex);
  REGISTER_ARG(m_repeatGroup);
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
  m_reflect = false;

  // don't start the gap timer till we're in a gap
  m_gapTimer.init(TIMER_1_ALARM, m_gapDuration);

  // start the blink timer now
  m_blinkTimer.init(TIMER_2_ALARMS | TIMER_START, m_onDuration, m_offDuration);

  if (!m_groupSize) {
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
  } else if (m_blinkTimer.curAlarm() == 1 && m_blinkTimer.onEnd() && m_colorset.onEnd() && !m_realGroupSize) {
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
  if (!m_repeatCounter) {
    m_repeatCounter = m_repeatGroup;
  }
}

void BasicPattern::onBlinkOn()
{
  // set the target led with the given color
  Leds::setIndex(m_ledPos, m_reflect ? m_colorset.getNext() : m_colorset.getPrev());
}

void BasicPattern::onBlinkOff()
{
  if (m_offDuration > 0) {
    // clear the target led if there is an off duration
    Leds::clearIndex(m_ledPos);
  }

  // count a blink in the group
  m_groupCounter++;

  if (m_reflectIndex && m_reflectIndex == m_groupCounter) {
    // switch direction of colors
    m_reflect = !m_reflect;
  }

  // check if the group has reached the intended size
  if (m_groupCounter >= m_realGroupSize && !m_reflectIndex) {
    triggerGap();
  } else if (m_groupCounter == (m_reflectIndex * 2) - 1) {
    triggerGap();
  }
}
