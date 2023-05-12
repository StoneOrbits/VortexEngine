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

  virtual void triggerDash();
  virtual void endDash();

  // whether in the gap
  bool inGap() const { return m_state == STATE_GAP; }

  // the parameters of the pattern
  uint8_t m_onDuration;
  uint8_t m_offDuration;
  uint8_t m_gapDuration;
  uint8_t m_dashDuration;

  // the various different blinking states the pattern can be in
  enum PatternState : uint8_t
  {
    // the pattern is blinking on the next color in the set
    STATE_BLINK_ON,
    // the pattern is displaying a color
    STATE_ON,
    // the pattern is blinking off
    STATE_BLINK_OFF,
    // the pattern is off
    STATE_OFF,
    // the pattern is starting a gap after a colorset
    STATE_BEGIN_GAP,
    // the pattern is off for an extended gap between colorsets
    STATE_IN_GAP,
    // the pattern is beginning a dash after a colorset or gap
    STATE_BEGIN_DASH,
    // the pattern is displaying a dash with the first color in the colorset
    STATE_DASH,
  };

  // the state of the current pattern
  PatternState m_state;

  // the blink timer used to measure blink timings
  Timer m_blinkTimer;
  // the secondary timer used to time gaps and dashes
  Timer m_altTimer;
};

#endif
