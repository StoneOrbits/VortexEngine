#ifndef DRIPMORPH_PATTERN_H
#define DRIPMORPH_PATTERN_H

#include "MultiLedPattern.h"\

#include "../../Timer.h"

class DripMorphPattern : public MultiLedPattern
{
public:
  DripMorphPattern();
  virtual ~DripMorphPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

protected:
  virtual void blinkOn();  // when the leds blink on
  virtual void blinkOff(); // when the leds blink off

private:  
  uint8_t m_blinkOnDuration;
  uint8_t m_blinkOffDuration;
  Timer m_blinkTimer;
  // the speed of the blend
  uint8_t m_speed;

  // current color and target blend color
  HSVColor m_cur;
  HSVColor m_next;
};

#endif

