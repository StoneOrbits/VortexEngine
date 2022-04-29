#include "TracerPattern.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Log.h"

TracerPattern::TracerPattern(uint32_t tracerLength, uint32_t dotLength) :
  m_tracerDuration(tracerLength),
  m_totalDuration(tracerLength + dotLength),
  m_tracerCounter(0),
  m_state(false)
{
}

TracerPattern::~TracerPattern()
{
}

// pure virtual must override the play function
void TracerPattern::play(Colorset *colorset, LedPos pos)
{
  if (!colorset) {
    // programmer error
    return;
  }

  // how far into a full frame this tick is
  uint32_t frameTime = Time::getCurtime(pos) % m_totalDuration;

  // whether the light should be on based on curtime
  bool state = (frameTime < m_tracerDuration);

  // if the state hasn't changed then nothing to do
  if (state == m_state) {
    return;
  }

  // the state changed
  m_state = state;

  if (state) {
    Leds::setIndex(pos, colorset->get(0));
  } else {
    // set the color of the tracer counter
    Leds::setIndex(pos, colorset->get(1 + m_tracerCounter));
    // increment tracer counter and wrap at 1 less than num colors
    m_tracerCounter = (m_tracerCounter + 1) % (colorset->numColors() - 1);
  }
}

// must override the serialize routine to save the pattern
void TracerPattern::serialize() const
{
  Pattern::serialize();
  Serial.print(m_tracerDuration);
  Serial.print(m_totalDuration - m_tracerDuration);
}

// must override unserialize to load patterns
void TracerPattern::unserialize()
{
}
