#ifndef SOLID_PATTERN_H
#define SOLID_PATTERN_H

#include "BasicPattern.h"

#include "../../Time/Timings.h"

class SolidPattern : public BasicPattern
{
public:
  SolidPattern(const PatternArgs &args);
  virtual ~SolidPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

protected:
  // callbacks for blinking on/off, can be overridden by derived classes
  virtual void onBlinkOn() override;

  uint8_t m_colIndex;
};

#endif
