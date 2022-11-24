#ifndef BRACKETS_PATTERN_H
#define BRACKETS_PATTERN_H

#include <inttypes.h>

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class BracketsPattern : public SingleLedPattern
{
public:
  BracketsPattern(uint8_t bracketDuration = 2, uint8_t midDuration = 5, uint8_t gapDuration = 8);
  virtual ~BracketsPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(ByteStream &buffer) const override;
  virtual void unserialize(ByteStream &buffer) override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

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
