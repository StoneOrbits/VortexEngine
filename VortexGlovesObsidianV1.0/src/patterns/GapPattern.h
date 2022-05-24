#ifndef GAP_PATTERN_H
#define GAP_PATTERN_H

#include "BasicPattern.h"

// a Gap pattern is a basic pattern + a gap
class GapPattern : public BasicPattern
{
public:
  GapPattern(uint32_t onDuration, uint32_t offDuration, uint32_t gapDuration);
  virtual ~GapPattern();

  virtual void init();

  virtual void play();

  virtual void serialize(SerialBuffer &buffer) const;
  virtual void unserialize(SerialBuffer &buffer);

protected:
  // override basicpattern callback
  virtual void onBasicEnd() override;

  // trigger the gap
  virtual void triggerGap();
  virtual void endGap();

  // whether in the gap
  bool inGap() const { return m_inGap; }

  uint32_t m_gapDuration;
  Timer m_gapTimer;
  bool m_inGap;
};

#endif
