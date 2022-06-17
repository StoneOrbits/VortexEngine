#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

#include <inttypes.h>

#include "BasicPattern.h"

class BlendPattern : public BasicPattern
{
public:
  BlendPattern(uint8_t onDuration = 2, uint8_t offDuration = 13, uint8_t speed = 1);
  virtual ~BlendPattern();

  virtual void init();

  virtual void play();

  // not sure these are necessary in derived classes anymore
  //virtual void serialize(SerialBuffer &buffer) const;
  //virtual void unserialize(SerialBuffer &buffer);

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

