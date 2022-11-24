#include "BlinkStepPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

BlinkStepPattern::BlinkStepPattern(uint8_t blinkOn, uint8_t blinkOff, uint8_t stepDuration) :
  MultiLedPattern(),
  m_blinkOnDuration(blinkOn),
  m_blinkOffDuration(blinkOff),
  m_stepDuration(stepDuration),
  m_blinkTimer(),
  m_stepTimer()
{
}

BlinkStepPattern::~BlinkStepPattern()
{
}

// init the pattern to initial state
void BlinkStepPattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(m_blinkOnDuration);
  m_blinkTimer.addAlarm(m_blinkOffDuration);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepDuration);
  m_stepTimer.start();
}

// pure virtual must override the play function
void BlinkStepPattern::play()
{
  // whether to trigger a step event based on the step timer
  bool shouldStep = (m_stepTimer.alarm() == 0);
  if (shouldStep) {
    prestep();
  }
  switch (m_blinkTimer.alarm()) {
  case -1: // just break and still run post-step
    break;
  case 0: // turn on the leds for given mapping
    blinkOn();
    break;
  case 1: // turn off the leds
    blinkOff();
    break;
  }
  if (shouldStep) {
    poststep();
  }
}

// must override the serialize routine to save the pattern
void BlinkStepPattern::serialize(ByteStream& buffer) const
{
  MultiLedPattern::serialize(buffer);
  buffer.serialize(m_blinkOnDuration);
  buffer.serialize(m_blinkOffDuration);
  buffer.serialize(m_stepDuration);
}

void BlinkStepPattern::unserialize(ByteStream& buffer)
{
  MultiLedPattern::unserialize(buffer);
  buffer.unserialize(&m_blinkOnDuration);
  buffer.unserialize(&m_blinkOffDuration);
  buffer.unserialize(&m_stepDuration);
}

#if SAVE_TEMPLATE == 1
void BlinkStepPattern::saveTemplate(int level) const
{
  MultiLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"BlinkOnDuration\": %d,", m_blinkOnDuration);
  IndentMsg(level + 1, "\"BlinkOffDuration\": %d,", m_blinkOffDuration);
  IndentMsg(level + 1, "\"StepDuration\": %d,", m_stepDuration);
}
#endif

void BlinkStepPattern::blinkOn()
{
  // override me
}

void BlinkStepPattern::blinkOff()
{
  // override me
  Leds::clearAll();
}

void BlinkStepPattern::prestep()
{
  // override me
}

void BlinkStepPattern::poststep()
{
  // override me
  m_colorset.getNext();
}
