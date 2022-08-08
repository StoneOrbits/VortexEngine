#ifndef ZIGZAG_PATTERN_H
#define ZIGZAG_PATTERN_h

#include "multiledpattern.h"

#include "../../Timer.h"

// There just happens to be LED_COUNT steps in the pattern
#define NUM_ZIGZAG_STEPS LED_COUNT
#define HALF_ZIGZAG_STEPS (NUM_ZIGZAG_STEPS / 2)
// step 8 is right after the middle finger
#define ZIGZAG_CHANGE_STEP 3

class ZigzagPattern : public MultiLedPattern
{
public:
  ZigzagPattern(uint8_t stepDuration = 50, uint8_t snakeSize = 1, uint8_t fadeAmount = 55);
  virtual ~ZigzagPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:

  class Snake {
  public:
    Snake(uint8_t step, uint8_t snakeSize, uint8_t fadeAmount, uint8_t changeBoundary);

    void init(uint32_t onDuration, uint32_t offDuration, const Colorset &colorset, uint32_t colorOffset);
    void step();
    void draw();

  private:
    void drawSnake();

    Timer m_blinkTimer;
    Colorset m_colorset;
    uint8_t m_step;
    uint8_t m_snakeSize;
    uint8_t m_fadeAmount;
    uint8_t m_changeBoundary;
  };

  // path for leds to take, index this with m_step up to LED_COUNT steps
  static const LedPos ledStepPositions[NUM_ZIGZAG_STEPS];

  // how long each step takes
  uint8_t m_stepDuration;

  // the step timer
  Timer m_stepTimer;

  // the two snakes that zigzag
  Snake m_snake1;
  Snake m_snake2;
};

#endif
