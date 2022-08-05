#ifndef ZIGZAG_PATTERN_H
#define ZIGZAG_PATTERN_h

#include "multiledpattern.h"

#include "../../Timer.h"

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
  bool m_fade;
  Finger m_step;
  uint8_t m_stepDuration;

  Timer m_blinkTimer;
  Timer m_stepTimer;
};
#endif