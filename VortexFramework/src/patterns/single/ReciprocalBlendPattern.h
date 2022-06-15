#ifndef RECIPROCAL_BLEND_PATTERN_H
#define RECIPROCAL_BLEND_PATTERN_H

#include <inttypes.h>

#include "BlendPattern.h"

class ReciprocalBlendPattern : public BlendPattern
{
public:
  ReciprocalBlendPattern(uint8_t onDuration = 2, uint8_t offDuration = 13, uint8_t speed = 1);
  virtual ~ReciprocalBlendPattern();

  virtual void init();

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;
  bool m_showingComplement;
};

#endif


