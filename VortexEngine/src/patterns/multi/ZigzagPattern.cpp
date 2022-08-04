#include "ZigzagPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

ZigzagPattern::ZigzagPattern(bool fade) :
  MultiLedPattern(),
  m_fade(fade),
  m_step(FINGER_FIRST),
  m_blinkTimer(),
  m_stepTimer()
{
}

ZigzagPattern::~ZigzagPattern()
{
}

// init the pattern to initial state
void ZigzagPattern::init()
{
  MultiLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();
  // dops timing
  m_blinkTimer.addAlarm(2);
  m_blinkTimer.addAlarm(13);
  // start the blink timer from the next frame
  m_blinkTimer.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(50);
  m_stepTimer.start();

  // start on the first color so that cur() works immediately
  m_colorset.setCurIndex(0);
}

// pure virtual must override the play function
void ZigzagPattern::play()
{
  // increment to the next step
  if (m_stepTimer.alarm() == 0) {
    m_step = (Finger)((m_step + 1) % 10);
    if (m_step == 8) {
      m_colorset.getNext();
    }
  }

  int alm = m_blinkTimer.alarm();
  // if first alarm is not triggering
  if (alm == -1) {
    // just return
    return;
  }

  // if starting the 'off' phase then turn the leds off
  if (alm == 1) {
    Leds::clearAll();
    return;
  }

  // otherwise first alarm is triggering
  // otherwise alarm is 0, starting the 'on' phase' 
  // Sets the number of loops based on the bool fade
  for (int snakeSize = 0; snakeSize < 4; snakeSize++) {
    // Step backwards to calculate trail
    Finger backStep = Finger((m_step + snakeSize) % 10);
    
    // determine two target leds
    LedPos target1 = (backStep < 5) ? fingerTip(backStep) : fingerTop((Finger)((LED_COUNT - backStep) - 1));
    LedPos target2 = (backStep < 5) ? fingerTop((Finger)((5 - backStep) - 1)) : fingerTip((Finger)((backStep - 5)));    
    
    // turn on target leds with current color
    Leds::setIndex(target1, m_colorset.cur());
    Leds::setIndex(target2, m_colorset.peekNext());
    if (!m_fade) {
      break;
    }
    // Dim each segment
    if (snakeSize > 0) {
      Leds::adjustBrightnessAll(75);         
    }
  }
}

// must override the serialize routine to save the pattern
void ZigzagPattern::serialize(SerialBuffer& buffer) const
{
  MultiLedPattern::serialize(buffer);
}

void ZigzagPattern::unserialize(SerialBuffer& buffer)
{
  MultiLedPattern::unserialize(buffer);
}
