#ifndef BRACKETS_PATTERN_H
#define BRACKETS_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class BracketsPattern : public SingleLedPattern
{
public:
  BracketsPattern(uint8_t bracketDuration = 4, uint8_t midDuration = 8, uint8_t gapDuration = 35);
  BracketsPattern(const PatternArgs &args);
  virtual ~BracketsPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

protected:
  // the duration of the brackets
  uint8_t m_bracketDuration;
  // the duration of the mid blink
  uint8_t m_midDuration;
  // the duration of the gap
  uint8_t m_offDuration;

  // the blink timer
  Timer m_blinkTimer;
};

#endif
