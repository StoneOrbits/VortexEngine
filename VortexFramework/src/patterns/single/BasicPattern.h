#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include <inttypes.h>

#include "SingleLedPattern.h"

#include "../../Timer.h"

class BasicPattern : public SingleLedPattern
{
public:
  BasicPattern(uint8_t onDuration, uint8_t offDuration = 0);
  virtual ~BasicPattern();

  virtual void init();

  virtual void play();

  virtual void serialize(SerialBuffer &buffer) const;
  virtual void unserialize(SerialBuffer &buffer);

protected:
  // callbacks for blinking on/off, can be overridden by derived classes
  virtual void onBlinkOn();
  virtual void onBlinkOff();

  // callbacks for when the pattern starts/stops
  virtual void onBasicStart();
  virtual void onBasicEnd();

  // the duration the light is on/off for
  uint8_t m_onDuration;
  uint8_t m_offDuration;

  // the blink timer
  Timer m_blinkTimer;
};

#endif
