#ifndef ADVANCED_PATTERN_H
#define ADVANCED_PATTERN_H

#include <inttypes.h>

#include "BasicPattern.h"

class AdvancedPattern : public BasicPattern
{
public:
  AdvancedPattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration,
    uint8_t groupSize, uint8_t skipCols, uint8_t repeatGroup);
  virtual ~AdvancedPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

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

  // the counter for groups
  uint8_t m_groupCounter;
  // the repeat counter
  uint8_t m_repeatCounter;
};

#endif
