#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

#include <inttypes.h>

#include "BasicPattern.h"

class BlendPattern : public BasicPattern
{
public:
  BlendPattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t speed = 1);
  virtual ~BlendPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;

  // the speed of the blend
  uint8_t m_speed;

  // current color and target blend color
  HSVColor m_cur;
  HSVColor m_next;
};

#endif

