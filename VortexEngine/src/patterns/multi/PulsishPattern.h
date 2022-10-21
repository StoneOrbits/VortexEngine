#include "MultiLedPattern.h"

#include "../../Timings.h"
#include "../../Timer.h"

class PulsishPattern : public MultiLedPattern
{
public:
  PulsishPattern(uint8_t onDuration1 = DOPISH_ON_DURATION, uint8_t offDuration1 = DOPISH_OFF_DURATION,
    uint8_t onDuration2 = STROBE_ON_DURATION, uint8_t offDuration2 = STROBE_OFF_DURATION, uint8_t stepDuration = 100);
  virtual ~PulsishPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  // how much the warp has progressed
  uint8_t m_progress;

  // the step duration
  uint8_t m_stepDuration;
  // the step timer
  Timer m_stepTimer;
  // the on and off durations
  uint8_t m_onDuration1;
  uint8_t m_offDuration1;
  uint8_t m_onDuration2;
  uint8_t m_offDuration2;
  // the blink timer
  Timer m_blinkTimer;
  Timer m_blink2Timer;
};
