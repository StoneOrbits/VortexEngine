#ifndef ZIGZAG_PATTERN_H
#define ZIGZAG_PATTERN_h

#include "multiledpattern.h"

#include "../../Timer.h"

// There just happens to be LED_COUNT steps in the pattern
#define NUM_ZIGZAG_STEPS LED_COUNT
#define HALF_ZIGZAG_STEPS (NUM_ZIGZAG_STEPS / 2)
// step 8 is right after the middle finger
#define ZIGZAG_CHANGE_STEP 8

class ZigzagPattern : public MultiLedPattern
{
public:
  ZigzagPattern(uint8_t stepDuration = 50, bool fade = false);
  virtual ~ZigzagPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  void drawSnake(uint32_t size, uint8_t step, uint32_t colIndex);

  // path for leds to take, index this with m_step up to LED_COUNT steps
  static const LedPos ledStepPositions[NUM_ZIGZAG_STEPS];

  bool m_fade;
  uint8_t m_step;

  Timer m_blinkTimer;
  Timer m_stepTimer;
};
#endif
