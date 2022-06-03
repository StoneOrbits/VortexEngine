#ifndef HUE_SHIFT_PATTERN_H
#define HUE_SHIFT_PATTERN_H

#include "MultiLedPattern.h"

class HueShiftPattern : public MultiLedPattern
{
public:
  HueShiftPattern(uint8_t scale, uint8_t speed);
  virtual ~HueShiftPattern();

  // init the pattern to initial state
  virtual void init();

  // pure virtual must override the play function
  virtual void play();

private:
  uint8_t m_speed;
  uint8_t m_scale;
};
#endif
