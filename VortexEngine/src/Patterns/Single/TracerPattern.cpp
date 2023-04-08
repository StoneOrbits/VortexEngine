#include "TracerPattern.h"

#include "../../Leds/Leds.h"

TracerPattern::TracerPattern(const PatternArgs &args) :
  SingleLedPattern(args),
  m_tracerDuration(0),
  m_dotDuration(0),
  m_blinkTimer(),
  m_dotColor(0)
{
  m_patternID = PATTERN_TRACER;
  REGISTER_ARG(m_tracerDuration);
  REGISTER_ARG(m_dotDuration);
  setArgs(args);
}

TracerPattern::~TracerPattern()
{
}

void TracerPattern::init()
{
  // run base pattern init logic
  SingleLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_tracerDuration);
  m_blinkTimer.addAlarm(m_dotDuration);

  // start the blink timer from the current frame
  m_blinkTimer.start();
}

// pure virtual must override the play function
void TracerPattern::play()
{
  AlarmID id = m_blinkTimer.alarm();
  if (id == 0) {
    // draw the tracer background
    Leds::setIndex(m_ledPos, m_colorset.get(0));
  } else if (id == 1) {
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
