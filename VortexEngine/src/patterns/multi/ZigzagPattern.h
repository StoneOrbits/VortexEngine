#ifndef ZIGZAG_PATTERN_H
#define ZIGZAG_PATTERN_h

#include "multiledpattern.h"

#include "../../Timer.h"

class ZigzagPattern : public MultiLedPattern
{
public:
  ZigzagPattern(bool fade = false);
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

  Timer m_blinkTimer;
  Timer m_stepTimer;
};
#endif