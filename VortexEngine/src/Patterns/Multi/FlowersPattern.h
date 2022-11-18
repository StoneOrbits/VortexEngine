#ifndef FLOWERS_PATTERN_H
#define FLOWERS_PATTERN_H

#include "HybridPattern.h"

// Rabbit is PATTERN_STROBIE on tops and PATTERN_STROBE on tips
class FlowersPattern : public HybridPattern
{
public:
  FlowersPattern(uint8_t onDuration1 = 5, uint8_t offDuration1 = 25, uint8_t onDuration2 = 3, uint8_t offDuration2 = 5);
  virtual ~FlowersPattern();

  // init the pattern to initial state
  virtual void init() override;

private:
  uint8_t m_onDuration1;
  uint8_t m_offDuration1;
  uint8_t m_onDuration2;
  uint8_t m_offDuration2;
};

#endif 