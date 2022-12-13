#ifndef COMPLEMENTARY_BLEND_PATTERN_H
#define COMPLEMENTARY_BLEND_PATTERN_H

#include <inttypes.h>

#include "BlendPattern.h"

class ComplementaryBlendPattern : public BlendPattern
{
public:
  ComplementaryBlendPattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION,
    uint8_t gapDuration = 0, uint8_t speed = 1);
; ComplementaryBlendPattern(const PatternArgs &args);
  virtual ~ComplementaryBlendPattern();

  virtual void init() override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;
  bool m_showingComplement;
};

#endif


