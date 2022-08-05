#include "ZigzagPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

// Mapping of LED positions to steps.
// The lights runs across tips, then back across tops.
// Index this array with m_step in order to get correct LedPos
const LedPos ZigzagPattern::ledStepPositions[NUM_ZIGZAG_STEPS] = {
  PINKIE_TIP,
  RING_TIP,
  MIDDLE_TIP,
  INDEX_TIP,
  THUMB_TIP,
  THUMB_TOP,
  INDEX_TOP,
  MIDDLE_TOP,
  RING_TOP,
  PINKIE_TOP,

  // PaLm LiGhTs?>??!?
#ifdef USE_PALM_LIGHTS
  PALM_LEFT,
  PALM_UP,
  PALM_RIGHT,
  PALM_DOWN
#endif
};

ZigzagPattern::ZigzagPattern(bool fade) :
  MultiLedPattern(),
  m_fade(fade),
  m_step(FINGER_FIRST),
  m_stepDuration(stepDuration),
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
  m_stepTimer.addAlarm(m_stepDuration);
  m_stepTimer.start();

  // start on the first color so that cur() works immediately
  m_colorset.setCurIndex(0);
}

// pure virtual must override the play function
void ZigzagPattern::play()
{
  // increment to the next step
  if (m_stepTimer.alarm() == 0) {
    m_step = (m_step + 1) % NUM_ZIGZAG_STEPS;
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

  // draw two snakes, one on the current step, one on the opposite side
  drawSnake(4, m_step, 0);
  drawSnake(4, (m_step + HALF_ZIGZAG_STEPS) % NUM_ZIGZAG_STEPS, 1);
}

void ZigzagPattern::drawSnake(uint32_t size, uint8_t step, uint32_t colIndex)
{
  uint8_t offset = 0;
  do {
    uint8_t section_step = 0;
    if (offset > step) {
      section_step = (NUM_ZIGZAG_STEPS + step) - offset;
    } else {
      section_step = step - offset;
    }
    LedPos target = ledStepPositions[section_step];
    RGBColor col = m_colorset.peek(colIndex);
    // step 8 is right after the middle finger
    if (offset > 0 && step >= ZIGZAG_CHANGE_STEP && 
      section_step < ZIGZAG_CHANGE_STEP) {
      col = m_colorset.peek(colIndex - 1);
    }
    // turn on target leds with current color
    Leds::setIndex(target, col);
    // Dim each segment after the first
    if (offset > 0) {
      Leds::adjustBrightnessIndex(target, 55 * offset);
    }
  } while (m_fade && ++offset < size);
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
