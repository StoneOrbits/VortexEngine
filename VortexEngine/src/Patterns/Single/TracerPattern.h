#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "../Pattern.h"

#include "../../Time/Timer.h"

class TracerPattern : public Pattern
{
public:
  TracerPattern(const PatternArgs &args);
  virtual ~TracerPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

private:
  // the duration the light is on/off for
  uint8_t m_tracerDuration;
  uint8_t m_dotDuration;
  // the timer for performing blinks
  Timer m_blinkTimer;
  // the counter for dot color
  uint8_t m_dotColor;
};

#endif
