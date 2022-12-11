#ifndef TRACER_PATTERN_H
#define TRACER_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class TracerPattern : public SingleLedPattern
{
public:
  TracerPattern(uint8_t tracerLength = 20, uint8_t dotLength = 1);
; TracerPattern(const PatternArgs &args);
  virtual ~TracerPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(ByteStream &buffer) const override;
  virtual void unserialize(ByteStream &buffer) override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
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
