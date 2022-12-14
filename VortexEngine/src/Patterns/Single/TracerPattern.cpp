#include "TracerPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

TracerPattern::TracerPattern(uint8_t tracerLength, uint8_t dotLength) :
  SingleLedPattern(),
  m_tracerDuration(tracerLength),
  m_dotDuration(dotLength),
  m_blinkTimer(),
  m_dotColor(0)
{
  m_patternID = PATTERN_TRACER;
}

TracerPattern::TracerPattern(const PatternArgs &args) :
  TracerPattern()
{
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

// must override the serialize routine to save the pattern
void TracerPattern::serialize(ByteStream &buffer) const
{
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_tracerDuration);
  buffer.serialize(m_dotDuration);
}

// must override unserialize to load patterns
void TracerPattern::unserialize(ByteStream &buffer)
{
  SingleLedPattern::unserialize(buffer);
  buffer.unserialize(&m_tracerDuration);
  buffer.unserialize(&m_dotDuration);
}

void TracerPattern::setArgs(const PatternArgs &args)
{
  SingleLedPattern::setArgs(args);
  m_tracerDuration = args.arg1;
  m_dotDuration = args.arg2;
}

void TracerPattern::getArgs(PatternArgs &args) const
{
  SingleLedPattern::getArgs(args);
  args.arg1 = m_tracerDuration;
  args.arg2 = m_dotDuration;
  args.numArgs += 2;
}

#if SAVE_TEMPLATE == 1
void TracerPattern::saveTemplate(int level) const
{
  SingleLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"TracerDuration\": %d,", m_tracerDuration);
  IndentMsg(level + 1, "\"DotDuration\": %d,", m_dotDuration);
}
#endif
