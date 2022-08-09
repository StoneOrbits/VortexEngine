#ifndef FILL_PATTERN_H
#define FILL_PATTERN_h

#include "multiledpattern.h"

#include "../../Timer.h"

class FillPattern : public MultiLedPattern
{
public:
  FillPattern(uint8_t stepDuration = 50, uint8_t snakeSize = 1, uint8_t fadeAmount = 55);
  virtual ~FillPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  // how much the fill has progressed
  uint8_t m_progress;

  // the step timer
  Timer m_stepTimer;
  // the blink timer
  Timer m_blinkTimer;
};

#endif
