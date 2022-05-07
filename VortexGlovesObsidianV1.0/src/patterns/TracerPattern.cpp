#include "TracerPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

TracerPattern::TracerPattern(uint32_t tracerLength, uint32_t dotLength) :
  m_tracerDuration(tracerLength),
  m_dotDuration(dotLength),
  m_blinkTimer(),
  m_dotColor(0)
{
}

TracerPattern::~TracerPattern()
{
}

void TracerPattern::init(Colorset *colorset, LedPos pos)
{
  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_tracerDuration);
  m_blinkTimer.addAlarm(m_dotDuration);

  // start the blink timer from the current frame
  m_blinkTimer.start();

  // run base pattern init logic
  Pattern::init(colorset, pos);
}

// pure virtual must override the play function
void TracerPattern::play()
{
  AlarmID id = m_blinkTimer.alarm();
  if (id == 0) {
    // display dot, never display the tracer color which 
    // is at index 0 of the colorset
    Leds::setIndex(m_ledPos, m_pColorset->get(1 + m_dotColor));
    // increment tracer counter and wrap at 1 less than num colors
    m_dotColor = (m_dotColor + 1) % (m_pColorset->numColors() - 1);
  } else if (id == 1) {
    // draw the tracer background
    Leds::setIndex(m_ledPos, m_pColorset->get(0));
  }
}

// must override the serialize routine to save the pattern
void TracerPattern::serialize() const
{
  Pattern::serialize();
  Serial.print(m_tracerDuration);
  Serial.print(m_dotDuration);
}

// must override unserialize to load patterns
void TracerPattern::unserialize()
{
}
