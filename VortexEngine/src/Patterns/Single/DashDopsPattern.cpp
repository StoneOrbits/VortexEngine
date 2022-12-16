#include "DashDopsPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

DashDopsPattern::DashDopsPattern(uint8_t dashLength, uint8_t dotLength, uint8_t offDuration) :
  SingleLedPattern(),
  m_dashDuration(dashLength),
  m_dotDuration(dotLength),
  m_offDuration(offDuration),
  m_blinkTimer(),
  m_dotColor(0)
{
  m_patternID = PATTERN_DASHDOPS;
}

DashDopsPattern::DashDopsPattern(const PatternArgs &args) :
  DashDopsPattern()
{
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

// must override the serialize routine to save the pattern
void DashDopsPattern::serialize(ByteStream& buffer) const
{
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_dashDuration);
  buffer.serialize(m_dotDuration);
  buffer.serialize(m_offDuration);
}

// must override unserialize to load patterns
void DashDopsPattern::unserialize(ByteStream& buffer)
{
  SingleLedPattern::unserialize(buffer);
  buffer.unserialize(&m_dashDuration);
  buffer.unserialize(&m_dotDuration);
  buffer.unserialize(&m_offDuration);
}

void DashDopsPattern::setArgs(const PatternArgs &args)
{
  SingleLedPattern::setArgs(args);
  m_dashDuration = args.arg1;
  m_dotDuration = args.arg2;
  m_offDuration = args.arg3;
}

void DashDopsPattern::getArgs(PatternArgs &args) const
{
  SingleLedPattern::getArgs(args);
  args.arg1 = m_dashDuration;
  args.arg2 = m_dotDuration;
  args.arg3 = m_offDuration;
  args.numArgs += 3;
}

#if SAVE_TEMPLATE == 1
void DashDopsPattern::saveTemplate(int level) const
{
  SingleLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"DashDuration\": %d,", m_dashDuration);
  IndentMsg(level + 1, "\"DotDuration\": %d,", m_dotDuration);
  IndentMsg(level + 1, "\"OffDuration\": %d,", m_offDuration);
}
#endif
