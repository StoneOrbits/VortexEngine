#include "BasicPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

BasicPattern::BasicPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration) :
  SingleLedPattern(),
  m_onDuration(onDuration),
  m_offDuration(offDuration),
  m_gapDuration(gapDuration),
  m_blinkTimer(),
  m_gapTimer(),
  m_inGap(false)
{
}

BasicPattern::~BasicPattern()
{
}

void BasicPattern::init()
{
  // run base pattern init logic
  SingleLedPattern::init();

  m_inGap = false;

  m_gapTimer.reset();
  m_gapTimer.addAlarm(m_gapDuration);

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_onDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the next frame
  m_blinkTimer.start();
}

void BasicPattern::play()
{
  if (m_inGap) {
    // check to see if the gap timer triggered to end the gap
    if (m_gapTimer.onEnd()) {
      endGap();
    }
    Leds::clearIndex(m_ledPos);
    return;
  }

  // check the alarm to toggle the light
  AlarmID id = m_blinkTimer.alarm();

  if (id == 0) {
    // when timer 0 starts it's time to blink on
    onBlinkOn();
  } else if (id == 1) {
    // when timer 1 starts it's time to blink off
    onBlinkOff();
  } else if (m_blinkTimer.curAlarm() == 1 && m_blinkTimer.onEnd() && m_colorset.onEnd()) {
    // callback for basic pattern ended
    onBasicEnd();
  }
}

void BasicPattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG_LOG("Serialize");
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_onDuration);
  buffer.serialize(m_offDuration);
  buffer.serialize(m_gapDuration);
}

void BasicPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG_LOG("Unserialize");
  SingleLedPattern::unserialize(buffer);
  buffer.unserialize(&m_onDuration);
  buffer.unserialize(&m_offDuration);
  buffer.unserialize(&m_gapDuration);
}

void BasicPattern::onBlinkOn()
{
  // if this is the first color in the colorset
  if (m_colorset.onStart()) {
    // run the pattern start callback
    onBasicStart();
  }
  // set the target led with the given color
  Leds::setIndex(m_ledPos, m_colorset.getNext());
}

void BasicPattern::onBlinkOff()
{
  if (m_offDuration > 0) {
    // clear the target led if there is an off duration
    Leds::clearIndex(m_ledPos);
  }
  // if there's no off duration and this is the last color
  if (!m_offDuration && m_colorset.onEnd()) {
    // then this is the end, run the pattern end callback
    onBasicEnd();
  }
}

void BasicPattern::onBasicStart()
{
}

// derived classes could override this to disable the gap feature
// and do something different on pattern end
void BasicPattern::onBasicEnd()
{
  triggerGap();
}

void BasicPattern::triggerGap()
{
  if (m_gapDuration > 0) {
    // next frame will be a gap
    m_gapTimer.restart(1);
    m_inGap = true;
  }
}

void BasicPattern::endGap()
{
  // next frame will not be a gap
  m_blinkTimer.restart(1);
  m_inGap = false;
}
