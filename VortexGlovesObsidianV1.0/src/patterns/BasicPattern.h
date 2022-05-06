#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include <inttypes.h>

#include "Pattern.h"

#include "../Timer.h"

class BasicPattern : public Pattern
{
public:
  BasicPattern(uint32_t onDuration, uint32_t offDuration = 0);
  virtual ~BasicPattern();

  virtual void init(Colorset *set, LedPos pos);

  virtual void play();

  virtual void resume();

  virtual void serialize() const;
  virtual void unserialize();

protected:
  // callbacks for blinking on/off, can be overridden by derived classes
  virtual void onBlinkOn();
  virtual void onBlinkOff();

  // callbacks for when the pattern starts/stops
  virtual void onBasicStart();
  virtual void onBasicEnd();

  // the duration the light is on/off for
  uint32_t m_onDuration;
  uint32_t m_offDuration;

  // the blink timer
  Timer m_blinkTimer;
};

#endif
