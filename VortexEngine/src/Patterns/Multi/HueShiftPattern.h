#ifndef HUE_SHIFT_PATTERN_H
#define HUE_SHIFT_PATTERN_H

#include "MultiLedPattern.h"

#include "../../Time/Timings.h"
#include "../../Time/Timer.h"

class HueShiftPattern : public MultiLedPattern
{
public:
  HueShiftPattern(uint8_t onDuration = STROBE_ON_DURATION, uint8_t offDuration = STROBE_OFF_DURATION);
  HueShiftPattern(const PatternArgs &args);
  virtual ~HueShiftPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream &buffer) const override;
  virtual void unserialize(ByteStream &buffer) override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

private:
  uint8_t m_blinkOnDuration;
  uint8_t m_blinkOffDuration;

  Timer m_blinkTimer;

  // current color and target blend color
  HSVColor m_cur;
  HSVColor m_next;
};
#endif
