#include "TracerPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

TracerPattern::TracerPattern(uint8_t tracerLength, uint8_t dotLength) :
  m_tracerDuration(tracerLength),
  m_dotDuration(dotLength),
  m_blinkTimer(),
  m_dotColor(0)
{
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

// must override the serialize routine to save the pattern
void TracerPattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG_LOG("Serialize");
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_tracerDuration);
  buffer.serialize(m_dotDuration);
}

// must override unserialize to load patterns
void TracerPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG_LOG("Unserialize");
  SingleLedPattern::unserialize(buffer);
  buffer.unserialize(&m_tracerDuration);
  buffer.unserialize(&m_dotDuration);
}
