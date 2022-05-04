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

  virtual void serialize() const;
  virtual void unserialize();

private:
  // the duration the light is on/off for
  uint32_t m_groupSize;
  uint32_t m_skipCols;
  uint32_t m_repeatGroup;
};

#endif
