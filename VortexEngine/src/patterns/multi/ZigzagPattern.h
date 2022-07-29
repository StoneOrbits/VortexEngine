#ifndef ZIGZAG_PATTERN_H
#define ZIGZAG_PATTERN_h

#include "multiledpattern.h"

class ZigzagPattern : public MultiLedPattern
{
public:
  ZigzagPattern(uint8_t speed = 1, uint8_t scale = 1);
  virtual ~ZigzagPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  uint8_t m_speed;
  uint8_t m_scale;
  uint8_t m_step = 0;
  
};
#endif