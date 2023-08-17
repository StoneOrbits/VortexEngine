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

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;

  enum InterpWrapType {
    INTERP_NO_WRAP,
    INTERP_WRAP
  };
  void interpolate(uint8_t &current, const uint8_t next, InterpWrapType wrap = INTERP_NO_WRAP);

  // offset of starting hue and number of flips
  uint8_t m_blendSpeed;
  uint8_t m_numFlips;
  uint8_t m_hueShift;

  // current color and target blend color
  HSVColor m_curHSV;
  HSVColor m_nextHSV;
  RGBColor m_curRGB;
  RGBColor m_nextRGB;

  // the current flip counter
  uint8_t m_flip;
};

#endif

