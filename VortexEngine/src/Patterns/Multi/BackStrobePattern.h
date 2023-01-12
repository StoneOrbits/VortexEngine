#ifndef BACKSTROBE_PATTERN_H
#define BACKSTROBE_PATTERN_H

#include "HybridPattern.h"
#include "../../Time/Timer.h"

class BackStrobePattern : public HybridPattern
{
public:
  BackStrobePattern(uint8_t onDuration1 = 4, uint8_t offDuration1 = 16, uint8_t gapDuration1 = 0,
    uint8_t onDuration2 = 16, uint8_t offDuration2 = 3, uint8_t gapDuration2 = 0, uint8_t stepSpeed100Ms = 10);
  BackStrobePattern(const PatternArgs &args);
  virtual ~BackStrobePattern();

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
  // the speed for the step timer in x100 ms chunks, so a value of 10
  // will be 1000ms
  uint8_t m_stepSpeed;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;

  PatternArgs m_firstPatternArgs;
  PatternArgs m_secondPatternArgs;
};

#endif 
