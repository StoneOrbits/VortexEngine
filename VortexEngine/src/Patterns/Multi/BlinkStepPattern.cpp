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
  // BlinkStep is an abstract class it cannot be directly
  // instantiated so we do not need to assign a pattern id
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

void BlinkStepPattern::setArgs(const PatternArgs &args)
{
  MultiLedPattern::setArgs(args);
  m_blinkOnDuration = args.arg1;
  m_blinkOffDuration = args.arg2;
  m_stepDuration = args.arg3;
}

void BlinkStepPattern::getArgs(PatternArgs &args) const
{
  MultiLedPattern::getArgs(args);
  args.arg1 = m_blinkOnDuration;
  args.arg2 = m_blinkOffDuration;
  args.arg3 = m_stepDuration;
  args.numArgs += 3;
}

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
