#ifndef HUE_SHIFT_PATTERN_H
#define HUE_SHIFT_PATTERN_H

#include "BasicPattern.h"

#include "../../Time/Timings.h"

class HueShiftPattern : public BasicPattern
{
public:
  HueShiftPattern(const PatternArgs &args);
  virtual ~HueShiftPattern();

  virtual void init() override;

protected:
  // only override the onBlinkOn so we can control the color it blinks
  virtual void onBlinkOn() override;
  void transitionValue(uint8_t &current, const uint8_t next, bool hue);

  // offset of starting hue and number of flips
  uint8_t m_shiftSpeed;
  uint8_t m_numFlips;

  // current color and target shift color
  HSVColor m_cur;
  HSVColor m_next;

  // the current flip counter
  uint8_t m_flip;
};

#endif

