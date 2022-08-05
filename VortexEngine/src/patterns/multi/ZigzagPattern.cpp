#include "ZigzagPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

// Mapping of LED positions to steps.
// The lights runs across tips, then back across tops.
// Index this array with m_step in order to get correct LedPos
const LedPos ZigzagPattern::ledStepPositions[NUM_ZIGZAG_STEPS] = {
  PINKIE_TOP,
  RING_TOP,
  MIDDLE_TOP,
  INDEX_TOP,
  THUMB_TOP,

  THUMB_TIP,
  INDEX_TIP,
  MIDDLE_TIP,
  RING_TIP,
  PINKIE_TIP,

  // PaLm LiGhTs?>??!?
#ifdef USE_PALM_LIGHTS
  PALM_LEFT,
  PALM_UP,
  PALM_RIGHT,
  PALM_DOWN
#endif
};

ZigzagPattern::ZigzagPattern(uint8_t stepDuration, uint8_t snakeSize, uint8_t fadeAmount) :
  MultiLedPattern(),
  m_stepDuration(stepDuration),
  m_snakeSize(snakeSize),
  m_fadeAmount(fadeAmount),
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
    if (m_step == ZIGZAG_CHANGE_STEP) {
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

  // draw two snakes, one on the current step, one on the opposite side
  drawSnake(m_snakeSize, m_step, 0);
  drawSnake(m_snakeSize, (m_step + HALF_ZIGZAG_STEPS) % NUM_ZIGZAG_STEPS, 1);
}

void ZigzagPattern::drawSnakeSegment(uint8_t position, uint32_t colIndex, uint32_t segment)
{
  LedPos target = ledStepPositions[position];
  RGBColor col = m_colorset.peek(colIndex);
  // turn on target leds with current color
  Leds::setIndex(target, col);
  // Dim each segment after the first
  if (segment < m_snakeSize) {
    Leds::adjustBrightnessIndex(target, m_fadeAmount * ((m_snakeSize - segment) - 1));
  }
}

void ZigzagPattern::drawSnake(uint32_t size, uint8_t head_position, uint32_t colIndex)
{
  for (uint8_t segment = 0; segment < size; ++segment) {
    // calculate the position of this segment
    uint8_t segment_position = (head_position + segment) % NUM_ZIGZAG_STEPS;
    // draw the segment of the snake (head first, then tail segments)
    drawSnakeSegment(segment_position, colIndex, segment);
    // if this segment is
    if (segment_position == ZIGZAG_CHANGE_STEP) {
      if (!colIndex) {
        colIndex = m_colorset.numColors() - 1;
      } else {
        colIndex--;
      }
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
