#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

#include <inttypes.h>

#include "BasicPattern.h"

#include "../../Time/Timings.h"

class BlendPattern : public BasicPattern
{
public:
  BlendPattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION,
    uint8_t gapDuration = 0, uint8_t offset = 0);
  BlendPattern(const PatternArgs &args);
  virtual ~BlendPattern();

  virtual void init() override;

  virtual void play() override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

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

