#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "Pattern.h"
#include "Timer.h"

class TracerPattern : public Pattern
{
public:
  TracerPattern(uint32_t tracerLength, uint32_t dotLength);
  virtual ~TracerPattern();

  virtual void init(Colorset *set, LedPos pos);

  virtual void play();

  virtual void serialize() const;
  virtual void unserialize();

private:
  // the duration the light is on/off for
  uint32_t m_tracerDuration;
  uint32_t m_dotDuration;
  // the timer for performing blinks
  Timer m_blinkTimer;
  // the counter for dot color
  uint32_t m_dotColor;
};

#endif
