#ifndef DRIPMORPH_PATTERN_H
#define DRIPMORPH_PATTERN_H

#include "MultiLedPattern.h"\

#include "../../Time/Timer.h"
#include "../../Time/Timings.h"


class DripMorphPattern : public MultiLedPattern
{
public:
  DripMorphPattern(uint8_t blinkOn = STROBE_ON_DURATION, uint8_t blinkOff = STROBE_OFF_DURATION, uint8_t speed = 1);
  DripMorphPattern(const PatternArgs &args);
  virtual ~DripMorphPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level) const override;
#endif

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

