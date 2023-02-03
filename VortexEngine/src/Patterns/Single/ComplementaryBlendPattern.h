#ifndef COMPLEMENTARY_BLEND_PATTERN_H
#define COMPLEMENTARY_BLEND_PATTERN_H

#include "BlendPattern.h"

class ComplementaryBlendPattern : public BlendPattern
{
public:
  ComplementaryBlendPattern(const PatternArgs &args);
  virtual ~ComplementaryBlendPattern();

  virtual void init() override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;

  bool m_showingComplement;
};

#endif


