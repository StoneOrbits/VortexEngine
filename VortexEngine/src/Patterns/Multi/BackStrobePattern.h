#ifndef BACKSTROBE_PATTERN_H
#define BACKSTROBE_PATTERN_H

#include "HybridPattern.h"
#include "../../Time/Timer.h"

class BackStrobePattern : public HybridPattern
{
public:
  BackStrobePattern(uint16_t stepSpeed = 1000);
; BackStrobePattern(const PatternArgs &args);
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
  // the speed for the step timer
  uint16_t m_stepSpeed;
  // the step timer
  Timer m_stepTimer;

  bool m_switch;
};

#endif 
