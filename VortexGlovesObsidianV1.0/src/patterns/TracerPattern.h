#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "Pattern.h"

class TracerPattern : public Pattern
{
public:
  TracerPattern(uint32_t tracerLength, uint32_t dotLength);
  virtual ~TracerPattern();

  virtual void play(Colorset *colorset, LedPos pos);

  virtual void serialize() const;
  virtual void unserialize();

private:

  uint32_t m_tracerDuration;
  uint32_t m_totalDuration;
  uint32_t m_tracerCounter;

  // the state of the light (only two states)
  bool m_state;
};

#endif
