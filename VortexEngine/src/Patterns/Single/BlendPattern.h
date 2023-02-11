#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

#include "AdvancedPattern.h"

#include "../../Time/Timings.h"

class BlendPattern : public AdvancedPattern
{
public:
  BlendPattern(const PatternArgs &args);
  virtual ~BlendPattern();

  virtual void init() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;
  void doBlink();
  void doFlip();

  // offset of starting hue and number of flips
  uint8_t m_hueOffset;
  uint8_t m_numFlips;

  // current color and target blend color
  HSVColor m_cur;
  HSVColor m_next;

  // the current flip counter
  uint8_t m_flip;
};

#endif

