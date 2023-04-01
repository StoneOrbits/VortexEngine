#ifndef BRACKETS_PATTERN_H
#define BRACKETS_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class BracketsPattern : public SingleLedPattern
{
public:
  BracketsPattern(const PatternArgs &args);
  virtual ~BracketsPattern();

  virtual void init() override;

  virtual void play() override;

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
