#ifndef ADVANCED_PATTERN_H
#define ADVANCED_PATTERN_H

#include "BasicPattern.h"

class AdvancedPattern : public BasicPattern
{
public:
  AdvancedPattern(uint8_t onDuration = 5, uint8_t offDuration = 5, uint8_t gapDuration = 10,
    uint8_t groupSize = 2, uint8_t skipCols = 2, uint8_t repeatGroup = 1);
  AdvancedPattern(const PatternArgs &args);
  virtual ~AdvancedPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

protected:
  // override from basicpattern
  virtual void onBasicEnd() override;
  virtual void onBlinkOff() override;

  // override from gappattern
  virtual void triggerGap() override;
  virtual void endGap() override;

  // the duration the light is on/off for
  uint8_t m_groupSize;
  uint8_t m_skipCols;
  uint8_t m_repeatGroup;

  // the real group size based on num colors
  uint8_t m_realGroupSize;
  // the counter for groups
  uint8_t m_groupCounter;
  // the repeat counter
  uint8_t m_repeatCounter;
};

#endif
