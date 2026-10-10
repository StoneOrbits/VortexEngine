#ifndef ZIGZAG_PATTERN_H
#define ZIGZAG_PATTERN_H

#include <stdbool.h>
#include "MultiLedPattern.h"

#include "../../Time/Timer.h"
#include "../../Colors/Colorset.h"

// a single snake, it owns a blink timer and its own colorset.
// Defined first so ZigzagPattern can embed it by value.
typedef struct ZigzagSnake {
  Timer m_blinkTimer;
  Colorset m_colorset;
  uint8_t m_step;
  uint8_t m_snakeSize;
  uint8_t m_fadeAmount;
  uint8_t m_changeBoundary;
} ZigzagSnake;

// ZigzagPattern runs two independent snakes across the led map, one heading
// over the evens and one over the odds.
typedef struct ZigzagPattern {
  // must remain the first member
  MultiLedPattern base;

  // blink on duration
  uint8_t m_onDuration;
  // blink off duration
  uint8_t m_offDuration;
  // how long each step takes
  uint8_t m_stepDuration;
  // the snake size
  uint8_t m_snakeSize;
  // the fade amount
  uint8_t m_fadeAmount;

  // the step timer
  Timer m_stepTimer;

  // the two snakes that zigzag
  ZigzagSnake m_snake1;
  ZigzagSnake m_snake2;
} ZigzagPattern;

// the led positions the snakes travel over, index with the snake step
extern const LedPos ZigzagPattern_ledStepPositions[];

// the vtable for this pattern
extern const PatternVtable ZigzagPattern_vtable;

void ZigzagPattern_init(ZigzagPattern *self, const PatternArgs *args);
void ZigzagPattern_destroy(Pattern *self);
void ZigzagPattern_initImpl(Pattern *self);
void ZigzagPattern_play(Pattern *self);

// snake entry points
void ZigzagSnake_make(ZigzagSnake *self);
void ZigzagSnake_init(ZigzagSnake *self, uint8_t onDuration, uint8_t offDuration, const Colorset *colorset,
  uint32_t colorOffset, uint8_t step, uint8_t snakeSize, uint8_t fadeAmount, uint8_t changeBoundary);
void ZigzagSnake_step(ZigzagSnake *self);
void ZigzagSnake_draw(ZigzagSnake *self);
void ZigzagSnake_drawSnake(ZigzagSnake *self);

#endif
