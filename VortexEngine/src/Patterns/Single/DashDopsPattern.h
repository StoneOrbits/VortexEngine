#ifndef DASHDOPS_PATTERN_H
#define DASHSOPS_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class DashDopsPattern : public SingleLedPattern
{
public:
  DashDopsPattern(uint8_t dashLength = 30, uint8_t dotLength = 2, uint8_t offDuration = 7);
  DashDopsPattern(const PatternArgs &args);
  virtual ~DashDopsPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  // the duration the light is on/off for
  uint8_t m_dashDuration;
  uint8_t m_dotDuration;
  uint8_t m_offDuration;
  // the timer for performing blinks
  Timer m_blinkTimer;
  // the counter for dot color
  uint8_t m_dotColor;
};

#endif
