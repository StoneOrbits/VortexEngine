#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Timer.h"

class TracerPattern : public SingleLedPattern
{
public:
  TracerPattern(uint8_t tracerLength = 20, uint8_t dotLength = 1);
  virtual ~TracerPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate() const override;
#endif

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
