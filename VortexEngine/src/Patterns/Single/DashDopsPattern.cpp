#include "DashDopsPattern.h"

#include "../../Leds/Leds.h"

DashDopsPattern::DashDopsPattern(const PatternArgs &args) :
  SingleLedPattern(args),
  m_dashDuration(0),
  m_dotDuration(0),
  m_offDuration(0),
  m_blinkTimer(),
  m_dotColor(0)
{
  m_patternID = PATTERN_DASHDOPS;
  REGISTER_ARG(m_dashDuration);
  REGISTER_ARG(m_dotDuration);
  REGISTER_ARG(m_offDuration);
  setArgs(args);
}

DashDopsPattern::~DashDopsPattern()
{
}

void DashDopsPattern::init()
{
  // run base pattern init logic
  SingleLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_dashDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  if (m_colorset.numColors() > 1) {
    for (uint8_t dots = 0; dots < m_colorset.numColors() - 1; ++dots) {
      m_blinkTimer.addAlarm(m_dotDuration);
      m_blinkTimer.addAlarm(m_offDuration);
    }
  }

  // start the blink timer from the current frame
  m_blinkTimer.start();
}

// pure virtual must override the play function
void DashDopsPattern::play()
{
  AlarmID id = m_blinkTimer.alarm();
  if (id == 0) {
    // draw the tracer background
    Leds::setIndex(m_ledPos, m_colorset.get(0));
  } else if (id % 2 == 1) {
    Leds::clearIndex(m_ledPos);
  } else if (id % 2 == 0) {
    // display dot, never display the tracer color which
    // is at index 0 of the colorset
    Leds::setIndex(m_ledPos, m_colorset.get(1 + m_dotColor));
    // prevent division by 0
    if (m_colorset.numColors() > 1) {
      // increment tracer counter and wrap at 1 less than num colors
      m_dotColor = (m_dotColor + 1) % (m_colorset.numColors() - 1);
    }
  }
}
