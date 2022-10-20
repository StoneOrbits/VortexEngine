#ifndef BACKSTROBE_PATTERN_H
#define BACKSTROBE_PATTERN_H

#include "HybridPattern.h"
#include "../../Timer.h"

class BackStrobePattern : public HybridPattern
{
public:
  BackStrobePattern(uint8_t switchSpeed = 1000);
  virtual ~BackStrobePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  uint8_t m_stepSpeed;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;
};

#endif 
