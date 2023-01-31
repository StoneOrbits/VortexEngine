#include "PulsishPattern.h"

#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"

PulsishPattern::PulsishPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2, uint8_t offDuration2, uint8_t stepDuration) :
  MultiLedPattern(),
  m_progress(),
  m_stepDuration(stepDuration),
  m_stepTimer(),
  m_onDuration1(onDuration1),
  m_offDuration1(offDuration1),
  m_onDuration2(onDuration2),
  m_offDuration2(offDuration2),
  m_blinkTimer(),
  m_blink2Timer()
{
  m_patternID = PATTERN_PULSISH;
}

PulsishPattern::PulsishPattern(const PatternArgs &args) :
  PulsishPattern()
{
  setArgs(args);
}

PulsishPattern::~PulsishPattern()
{
}

// init the pattern to initial state
void PulsishPattern::init()
{
  MultiLedPattern::init();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepDuration);
  m_stepTimer.start();

  // make blink timer
  m_blinkTimer.reset();
  m_blinkTimer.addAlarm(m_onDuration1);
  m_blinkTimer.addAlarm(m_offDuration1);
  m_blinkTimer.start();

  // make 2nd blink timer
  m_blink2Timer.reset();
  m_blink2Timer.addAlarm(m_onDuration2);
  m_blink2Timer.addAlarm(m_offDuration2);
  m_blink2Timer.start();
}
// pure virtual must override the play function
void PulsishPattern::play()
{
  // when the step timer triggers
  if (m_stepTimer.alarm() == 0) {
    m_progress = (m_progress + 1) % QUADRANT_LAST;
  }

  switch (m_blinkTimer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    for (Quadrant quadrant = QUADRANT_FIRST; quadrant < QUADRANT_COUNT; ++quadrant) {
      if (quadrant != m_progress) {
        Leds::setQuadrant(quadrant, m_colorset.cur());
      }
    }
    m_colorset.skip();
    if (m_colorset.curIndex() == 0) {
      m_colorset.skip();
    }
    break;
  case 1:
    for (Quadrant quadrant = QUADRANT_FIRST; quadrant < QUADRANT_COUNT; ++quadrant) {
      if (quadrant != m_progress) {
        Leds::clearQuadrant(quadrant);
      }
    }
    break;
  }

  switch (m_blink2Timer.alarm()) {
  case -1: // just return
    return;
  case 0: // turn on the leds
    Leds::setQuadrant((Quadrant)m_progress, m_colorset.get(0));
    break;
  case 1:
    Leds::clearQuadrant((Quadrant)m_progress);
    break;
  }
}

void PulsishPattern::setArgs(const PatternArgs &args)
{
  MultiLedPattern::setArgs(args);
  m_onDuration1 = args.arg1;
  m_offDuration1 = args.arg2;
  m_onDuration2 = args.arg3;
  m_offDuration2 = args.arg4;
  m_stepDuration = args.arg5;
}

void PulsishPattern::getArgs(PatternArgs &args) const
{
  MultiLedPattern::getArgs(args);
  args.arg1 = m_onDuration1;
  args.arg2 = m_offDuration1;
  args.arg3 = m_onDuration2;
  args.arg4 = m_offDuration2;
  args.arg5 = m_stepDuration;
  args.numArgs += 5;
}
