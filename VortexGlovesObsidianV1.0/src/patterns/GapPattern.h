#ifndef GAP_PATTERN_H
#define GAP_PATTERN_H

#include "BasicPattern.h"

// a Gap pattern is a basic pattern + a gap
class GapPattern : public BasicPattern
{
public:
  GapPattern(uint32_t onDuration, uint32_t offDuration, uint32_t gapDuration);
  virtual ~GapPattern();

  virtual void init(Colorset *set, LedPos pos);

  virtual void play();

  virtual void serialize() const;
  virtual void unserialize();

protected:
  // trigger the gap
  void triggerGap();
  void endGap();

  // whether in the gap
  bool inGap() const { return m_inGap; }

  uint32_t m_gapDuration;
  Timer m_gapTimer;
  bool m_inGap;
};

#endif
