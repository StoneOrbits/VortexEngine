#ifndef SPLITSTROBIE_PATTERN_H
#define SPLITSTROBIE_PATTERN_H

#include "HybridPattern.h"
#include "../../Time/Timer.h"
#include "../../Time/Timings.h"

class SplitStrobiePattern : public HybridPattern
{
public:
  SplitStrobiePattern(uint8_t onDuration = DOPS_ON_DURATION, uint8_t offDuration = DOPS_OFF_DURATION, uint8_t gapDuration = 0,
    uint8_t dashDuration = 16, uint8_t dotDuration = 3, uint8_t stepDuration100ms = 10);
  SplitStrobiePattern(const PatternArgs &args);
  virtual ~SplitStrobiePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream& buffer) const override;
  virtual void unserialize(ByteStream& buffer) override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

private:
  // the duration for the step in x100ms
  uint8_t m_stepDuration;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;

  PatternArgs m_firstPatternArgs;
  PatternArgs m_secondPatternArgs;
};

#endif 
