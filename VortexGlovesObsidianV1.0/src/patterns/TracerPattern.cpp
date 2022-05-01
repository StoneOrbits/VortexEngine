#include "TracerPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

TracerPattern::TracerPattern(uint32_t tracerLength, uint32_t dotLength) :
  m_tracerDuration(tracerLength),
  m_dotDuration(dotLength),
  m_blinkTimer()
{
}

TracerPattern::~TracerPattern()
{
}

void TracerPattern::init(Colorset *colorset, LedPos pos)
{
  // run base pattern init logic
  Pattern::init(pos);
  // reset the timer
  //m_blinkTimer.start();
  // add the alarms for on then off
}

// pure virtual must override the play function
void TracerPattern::play()
{
  if (!colorset) {
    // programmer error
    return;
  }

  // switch on the alarm result
  switch (m_blinkTimer.alarm()) {
  case ALARM_NONE:
  default: // the alarm did not trigger
    return;
  case 0: // the first alarm triggered, display ribbon color
    Leds::setIndex(m_ledPos, colorset->get(0));
    break;
  case 1: // the second alarm triggered, display dot
    Leds::setIndex(m_ledPos, colorset->get(1 + m_dotColor));
    // increment tracer counter and wrap at 1 less than num colors
    m_dotColor = (m_dotColor + 1) % (colorset->numColors() - 1);
    break;
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
