#include "TracerPattern.h"

#include "../../Leds/Leds.h"

TracerPattern::TracerPattern(const PatternArgs &args) :
  Pattern(args),
  m_tracerDuration(0),
  m_dotDuration(0),
  m_blinkTimer(),
  m_dotColor(0)
{
  m_patternID = PATTERN_TRACER;
  setArgs(args);
}

TracerPattern::~TracerPattern()
{
}

void TracerPattern::init()
{
  // run base pattern init logic
  Pattern::init();

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

void TracerPattern::setArgs(const PatternArgs &args)
{
  Pattern::setArgs(args);
  m_tracerDuration = args.arg1;
  m_dotDuration = args.arg2;
}

void TracerPattern::getArgs(PatternArgs &args) const
{
  Pattern::getArgs(args);
  args.arg1 = m_tracerDuration;
  args.arg2 = m_dotDuration;
  args.numArgs += 2;
}
