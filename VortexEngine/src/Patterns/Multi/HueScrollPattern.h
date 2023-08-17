#ifndef HUE_SCROLL_PATTERN_H
#define HUE_SCROLL_PATTERN_H

#include "MultiLedPattern.h"

#include "../../Time/Timings.h"
#include "../../Time/Timer.h"

class HueScrollPattern : public MultiLedPattern
{
public:
  HueScrollPattern(const PatternArgs &args);
  virtual ~HueScrollPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

private:
  uint8_t m_blinkOnDuration;
  uint8_t m_blinkOffDuration;

  Timer m_blinkTimer;

  // current color and target blend color
  HSVColor m_cur;
  HSVColor m_next;
};
#endif
