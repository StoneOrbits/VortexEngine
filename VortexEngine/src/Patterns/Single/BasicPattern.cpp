#include "BasicPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

BasicPattern::BasicPattern(const PatternArgs &args) :
  Pattern(args),
  m_onDuration(0),
  m_offDuration(0),
  m_gapDuration(0),
  m_dashDuration(0),
  m_state(STATE_BLINK_ON),
  m_blinkTimer(),
  m_altTimer()
{
  m_patternID = PATTERN_BASIC;
  REGISTER_ARG(m_onDuration);
  REGISTER_ARG(m_offDuration);
  REGISTER_ARG(m_gapDuration);
  REGISTER_ARG(m_dashDuration);
  setArgs(args);
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::init()
{
  // run base pattern init logic
  Pattern::init();

  // start the blink timer now
  m_blinkTimer.init(TIMER_2_ALARMS | TIMER_START, m_onDuration, m_offDuration);
}

void BasicPattern::play()
{
  switch (m_state) {
  case STATE_BLINK_ON:
    if (!m_onDuration) {
      m_state = STATE_BLINK_OFF;
      return;
    }
    onBlinkOn();
    m_state = STATE_ON;
    break;
  case STATE_ON:
    if (m_blinkTimer.alarm() != 0) {
      m_state = STATE_BLINK_OFF;
    }
    // just do nothing
    return;
  case STATE_BLINK_OFF:
    if (m_colorset.onEnd()) {
    }
    break;
  case STATE_DASH:
    break;
  }
}

void BasicPattern::onBlinkOn()
{
  // set the target led with the given color
  Leds::setIndex(m_ledPos, m_colorset.getNext());
}

void BasicPattern::onBlinkOff()
{
  if (m_offDuration > 0) {
    // clear the target led if there is an off duration
    Leds::clearIndex(m_ledPos);
  }
}
