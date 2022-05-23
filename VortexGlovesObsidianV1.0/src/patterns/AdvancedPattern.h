#ifndef ADVANCED_PATTERN_H
#define ADVANCED_PATTERN_H

#include <inttypes.h>

#include "GapPattern.h"

class AdvancedPattern : public GapPattern
{
public:
  AdvancedPattern(uint32_t onDuration, uint32_t offDuration, uint32_t gapDuration,
    uint32_t groupSize, uint32_t skipCols, uint32_t repeatGroup);
  virtual ~AdvancedPattern();

  virtual void init(Colorset *set, LedPos pos);

  virtual void play();

  virtual void serialize(SerialBuffer &buffer) const;
  virtual void unserialize(SerialBuffer &buffer);

protected:
  // override from basicpattern
  virtual void onBasicEnd() override;
  virtual void onBlinkOff() override;

  // override from gappattern
  virtual void triggerGap() override;
  virtual void endGap() override;

  // the duration the light is on/off for
  uint32_t m_groupSize;
  uint32_t m_skipCols;
  uint32_t m_repeatGroup;

  // the counter for groups
  uint32_t m_groupCounter;
  // the repeat counter
  uint32_t m_repeatCounter;
};

#endif
