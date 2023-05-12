#include "BasicPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

// uncomment me to print debug labels on the pattern states
//#define DEBUG_BASIC_PATTERN

#ifdef DEBUG_BASIC_PATTERN
#include <stdio.h>
#endif

BasicPattern::BasicPattern(const PatternArgs &args) :
  Pattern(args),
  m_onDuration(0),
  m_offDuration(0),
  m_gapDuration(0),
  m_dashDuration(0),
  m_state(STATE_BLINK_ON),
  m_blinkTimer()
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

  // if a dash is present then always start with the dash because
  // it consumes the first color in the colorset
  if (m_dashDuration > 0) {
    m_state = STATE_BEGIN_DASH;
  }
  // if there's no on duration or dash duration the led is just disabled
  if (!m_onDuration && !m_dashDuration) {
    m_state = STATE_DISABLED;
  }
}

void BasicPattern::nextState(uint8_t timing)
{
  m_blinkTimer.init(TIMER_1_ALARM | TIMER_START, timing);
  m_state = (PatternState)(m_state + 1);
}

void BasicPattern::play()
{

  // Sometimes the pattern needs to cycle multiple states in a single frame so
  // instead of using a loop or recursion I have just used a simple goto
replay:


  // its kinda evolving as i go
  switch (m_state) {
  case STATE_DISABLED:
    return;
  case STATE_BLINK_ON:
    if (m_onDuration > 0) { onBlinkOn(); nextState(m_onDuration); return; }
    m_state = STATE_BLINK_OFF;
  case STATE_BLINK_OFF:
    // the whole 'should blink off' situation is tricky because we might need
    // to go back to blinking on if or colorset isn't at the end yet
    if (!m_colorset.onEnd() || (!m_gapDuration && !m_dashDuration)) {
      if (m_offDuration > 0) { onBlinkOff(); nextState(m_offDuration); return; }
      if (!m_colorset.onEnd() && m_onDuration > 0) { m_state = STATE_BLINK_ON; goto replay; }
    }
    m_state = STATE_BEGIN_GAP;
  case STATE_BEGIN_GAP:
    if (m_gapDuration > 0) { beginGap(); nextState(m_gapDuration); return; }
    m_state = STATE_BEGIN_DASH;
  case STATE_BEGIN_DASH:
    if (m_dashDuration > 0) { beginDash(); nextState(m_dashDuration); return; }
    m_state = STATE_BEGIN_GAP2;
  case STATE_BEGIN_GAP2:
    if (m_dashDuration > 0 && m_gapDuration > 0) { beginGap(); nextState(m_gapDuration); return; }
    m_state = STATE_BLINK_ON;
    goto replay;
  default:
    break;
  }

  if (m_blinkTimer.alarm() == -1) {
#ifdef DEBUG_BASIC_PATTERN
    switch (m_state) {
    case STATE_ON: printf("on  "); break;
    case STATE_OFF: printf("off "); break;
    case STATE_IN_GAP: printf("gap1"); break;
    case STATE_IN_DASH: printf("dash"); break;
    case STATE_IN_GAP2: printf("gap2"); break;
    default: break;
    }
#endif
    // no alarm triggered?
    return;
  }

  if (m_state == STATE_IN_GAP2 || (m_state == STATE_OFF && !m_colorset.onEnd())) {
    if (!m_onDuration && !m_gapDuration) {
      m_state = STATE_BEGIN_DASH;
    } else {
      m_state = STATE_BLINK_ON;
    }
  } else if (m_state == STATE_OFF && (m_colorset.onEnd() || m_colorset.numColors() == 1)) {
    m_state = STATE_BEGIN_GAP;
  } else {
    m_state = (PatternState)(m_state + 1);
  }
  // recurse with the new state change
  goto replay;
}

void BasicPattern::onBlinkOn()
{
#ifdef DEBUG_BASIC_PATTERN
  printf("on  ");
#endif
  Leds::setIndex(m_ledPos, m_colorset.getNext());
}

void BasicPattern::onBlinkOff()
{
#ifdef DEBUG_BASIC_PATTERN
  printf("off ");
#endif
  Leds::clearIndex(m_ledPos);
}

void BasicPattern::beginGap()
{
#ifdef DEBUG_BASIC_PATTERN
  printf("gap%d", (m_state == STATE_BEGIN_GAP) ? 1 : 2);
#endif
  Leds::clearIndex(m_ledPos);
}

void BasicPattern::beginDash()
{
#ifdef DEBUG_BASIC_PATTERN
  printf("dash");
#endif
  Leds::setIndex(m_ledPos, m_colorset.getNext());
}
