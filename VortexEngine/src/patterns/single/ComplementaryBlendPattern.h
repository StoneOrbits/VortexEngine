#ifndef COMPLEMENTARY_BLEND_PATTERN_H
#define COMPLEMENTARY_BLEND_PATTERN_H

#include <inttypes.h>

#include "BlendPattern.h"

class ComplementaryBlendPattern : public BlendPattern
{
public:
  ComplementaryBlendPattern(uint8_t onDuration = 2, uint8_t offDuration = 13, uint8_t speed = 1);
  virtual ~ComplementaryBlendPattern();

  virtual void init() override;

  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;
  bool m_showingComplement;
};

#endif


