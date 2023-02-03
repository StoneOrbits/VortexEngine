#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

#include "BasicPattern.h"

#include "../../Time/Timings.h"

class BlendPattern : public BasicPattern
{
public:
  BlendPattern(const PatternArgs &args);
  virtual ~BlendPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;

  // offset of starting hue
  uint8_t m_hueOffset;

  // current color and target blend color
  HSVColor m_cur;
  HSVColor m_next;
};

#endif

