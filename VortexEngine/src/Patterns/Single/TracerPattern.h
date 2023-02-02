#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class TracerPattern : public SingleLedPattern
{
public:
  TracerPattern(uint8_t tracerLength = 8, uint8_t dotLength = 3);
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
