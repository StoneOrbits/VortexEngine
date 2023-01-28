#ifndef SOLID_PATTERN_H
#define SOLID_PATTERN_H

#include "BasicPattern.h"
#include "../../Time/Timings.h"

class SolidPattern : public BasicPattern
{
public:
  SolidPattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION,
    uint8_t gapDuration = 0, uint8_t colIndex = 0);
  SolidPattern(const PatternArgs &args);
  virtual ~SolidPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

protected:
  // callbacks for blinking on/off, can be overridden by derived classes
  virtual void onBlinkOn();

  uint8_t m_colIndex;
};

#endif
