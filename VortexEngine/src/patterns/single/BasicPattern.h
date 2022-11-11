#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include <inttypes.h>

#include "SingleLedPattern.h"

#include "../../Timer.h"

class BasicPattern : public SingleLedPattern
{
public:
  BasicPattern(uint8_t onDuration, uint8_t offDuration = 0, uint8_t gapDuration = 0);
  virtual ~BasicPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate(int level = 0) const override;
#endif

protected:
  // callbacks for blinking on/off, can be overridden by derived classes
  virtual void onBlinkOn();
  virtual void onBlinkOff();

  // callbacks for when the pattern starts/stops
  virtual void onBasicStart();
  virtual void onBasicEnd();

  // trigger the gap
  virtual void triggerGap();
  virtual void endGap();

  // whether in the gap
  bool inGap() const { return m_inGap; }

  // the duration the light is on/off for
  uint8_t m_onDuration;
  uint8_t m_offDuration;
  uint8_t m_gapDuration;

  // the blink timer
  Timer m_blinkTimer;
  Timer m_gapTimer;
  bool m_inGap;
};

#endif
