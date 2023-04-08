#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include "../Pattern.h"
#include "../../Time/Timer.h"

class BasicPattern : public Pattern
{
public:
  BasicPattern(const PatternArgs &args);
  virtual ~BasicPattern();

  virtual void init() override;

  virtual void play() override;

protected:
  // when blinking off
  virtual void onBlinkOn();
  virtual void onBlinkOff();

  // trigger the gap
  virtual void triggerGap();
  virtual void endGap();

  // whether in the gap
  bool inGap() const { return m_inGap; }

  // the duration the light is on/off for
  uint8_t m_onDuration;
  uint8_t m_offDuration;
  uint8_t m_gapDuration;
  // the duration the light is on/off for
  uint8_t m_groupSize;
  uint8_t m_skipCols;
  uint8_t m_repeatGroup;

  // the real group size based on num colors
  uint8_t m_realGroupSize;
  // the counter for groups
  uint8_t m_groupCounter;
  // the repeat counter
  uint8_t m_repeatCounter;

  // the blink timer
  Timer m_blinkTimer;
  Timer m_gapTimer;
  bool m_inGap;
};

#endif
