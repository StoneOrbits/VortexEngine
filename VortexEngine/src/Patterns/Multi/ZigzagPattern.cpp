#include "ZigzagPattern.h"

#include <string.h>

#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

// Mapping of LED positions to steps.
// The lights runs across evens, then back across odds.
// Index this array with m_step in order to get correct LedPos
const LedPos ZigzagPattern_ledStepPositions[] = {
  LED_1,
  LED_3,
  LED_5,
  LED_7,
  LED_9,

  LED_8,
  LED_6,
  LED_4,
  LED_2,
  LED_0,
};

// There just happens to be LED_COUNT steps in the pattern
#define NUM_ZIGZAG_STEPS (sizeof(ZigzagPattern_ledStepPositions) / sizeof(ZigzagPattern_ledStepPositions[0]))
#define HALF_ZIGZAG_STEPS (NUM_ZIGZAG_STEPS / 2)

const PatternVtable ZigzagPattern_vtable = {
  .destroy = ZigzagPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = ZigzagPattern_initImpl,
  .play = ZigzagPattern_play,
};

void ZigzagPattern_init(ZigzagPattern *self, const PatternArgs *args)
{
  MultiLedPattern_init(&self->base, args, &ZigzagPattern_vtable);
  self->m_onDuration = 0;
  self->m_offDuration = 0;
  self->m_stepDuration = 0;
  self->m_snakeSize = 0;
  self->m_fadeAmount = 0;
  Timer_make(&self->m_stepTimer);
  ZigzagSnake_make(&self->m_snake1);
  ZigzagSnake_make(&self->m_snake2);
  Pattern_setPatternID((&self->base.base), PATTERN_ZIGZAG);
  REGISTER_ARG((&self->base.base), ZigzagPattern, m_onDuration);
  REGISTER_ARG((&self->base.base), ZigzagPattern, m_offDuration);
  REGISTER_ARG((&self->base.base), ZigzagPattern, m_stepDuration);
  REGISTER_ARG((&self->base.base), ZigzagPattern, m_snakeSize);
  REGISTER_ARG((&self->base.base), ZigzagPattern, m_fadeAmount);
  Pattern_setArgs((&self->base.base), args);
}

void ZigzagPattern_destroy(Pattern *pat)
{
  ZigzagPattern *self = (ZigzagPattern *)pat;
  (void)self;
}

// init the pattern to initial state
void ZigzagPattern_initImpl(Pattern *pat)
{
  ZigzagPattern *self = (ZigzagPattern *)pat;
  MultiLedPattern_doInit(pat);

  // reset and add alarm
  Timer_reset(&self->m_stepTimer);
  Timer_addAlarm(&self->m_stepTimer, self->m_stepDuration);
  Timer_start0(&self->m_stepTimer);

  // initialize the snakes with dops timing
  ZigzagSnake_init(&self->m_snake1, self->m_onDuration, self->m_offDuration, &self->base.base.m_colorset,
    0, 0, self->m_snakeSize, self->m_fadeAmount, 3);
  ZigzagSnake_init(&self->m_snake2, self->m_onDuration, self->m_offDuration, &self->base.base.m_colorset,
    1, HALF_ZIGZAG_STEPS, self->m_snakeSize, self->m_fadeAmount, 8);
}

// pure virtual must override the play function
void ZigzagPattern_play(Pattern *pat)
{
  ZigzagPattern *self = (ZigzagPattern *)pat;
  // when the step timer triggers
  if (Timer_alarm(&self->m_stepTimer) == 0) {
    ZigzagSnake_step(&self->m_snake1);
    ZigzagSnake_step(&self->m_snake2);
  }

  ZigzagSnake_draw(&self->m_snake1);
  ZigzagSnake_draw(&self->m_snake2);
}

// ===================
//  Snake code

void ZigzagSnake_make(ZigzagSnake *self)
{
  memset(self, 0, sizeof(*self));
  Timer_make(&self->m_blinkTimer);
  self->m_step = 0;
  self->m_snakeSize = 1;
  self->m_fadeAmount = 55;
  self->m_changeBoundary = 3;
}

void ZigzagSnake_init(ZigzagSnake *self, uint8_t onDuration, uint8_t offDuration, const Colorset *colorset,
  uint32_t colorOffset, uint8_t step, uint8_t snakeSize, uint8_t fadeAmount, uint8_t changeBoundary)
{
  // reset the blink timer entirely
  Timer_reset(&self->m_blinkTimer);
  // dops timing
  Timer_addAlarm(&self->m_blinkTimer, onDuration);
  Timer_addAlarm(&self->m_blinkTimer, offDuration);
  // start the blink timer from the next frame
  Timer_start0(&self->m_blinkTimer);
  // start on the first color so that cur() works immediately
  self->m_colorset = *colorset;
  Colorset_setCurIndex(&self->m_colorset, colorOffset);
  // set the starting step
  self->m_step = step;
  // set the snake size
  self->m_snakeSize = snakeSize;
  // set fade amount and change boundary
  self->m_fadeAmount = fadeAmount;
  self->m_changeBoundary = changeBoundary;
}

void ZigzagSnake_step(ZigzagSnake *self)
{
  // increment to the next step
  self->m_step = (self->m_step + 1) % NUM_ZIGZAG_STEPS;
  // if the step is on a change boundary then increment the snake color
  if (self->m_step == self->m_changeBoundary) {
    Colorset_getNext(&self->m_colorset);
  }
}

void ZigzagSnake_draw(ZigzagSnake *self)
{
  // when the blinkTimer triggers
  switch (Timer_alarm(&self->m_blinkTimer)) {
  case 0: // blinking on
    // draw two snakes, one on the current step, one on the opposite side
    ZigzagSnake_drawSnake(self);
    break;
  case 1: // blinking off
    Leds_clearAll();
    break;
  default:
  case -1:
    // nothing
    break;
  }
}

void ZigzagSnake_drawSnake(ZigzagSnake *self)
{
  uint8_t segment_position;
  int32_t colIndex = 0;
  for (uint8_t segment = 0; segment < self->m_snakeSize; ++segment) {
    // calculate the position of this segment, if the segment being rendered is
    // greater than the head position it means the snake head is at an index that
    // won't support it's entire tail without wrapping below 0
    if (segment > self->m_step) {
      segment_position = NUM_ZIGZAG_STEPS - (segment - self->m_step);
    } else {
      segment_position = self->m_step - segment;
    }
    // adjust the brightness of the color before setting
    RGBColor col = Colorset_peek(&self->m_colorset, colIndex);
    if (segment < self->m_snakeSize) {
      RGBColor_adjustBrightness(&col, self->m_fadeAmount * segment);
    }
    // lookup the target in the step positions array and turn it on with given color/brightness
    Leds_setIndex(ZigzagPattern_ledStepPositions[segment_position], col);
    // if this segment is on the step where the color changes
    if (segment_position == self->m_changeBoundary) {
      // then decrement the color index for the rest of the snake so that the
      // tail of the snake has the old color till it reaches the change point
      colIndex--;
    }
  }
}
