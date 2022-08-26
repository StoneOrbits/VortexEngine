#ifndef BACKSTROBE_PATTERN_H
#define BACKSTROBE_PATTERN_H

#include "HybridPattern.h"
#include "../../Timer.h"

class BackStrobePattern : public HybridPattern
{
public:
  BackStrobePattern();
  virtual ~BackStrobePattern();

  // init the pattern to initial state
  virtual void init() override;
  virtual void play() override;

private:
  // the step timer
  Timer m_stepTimer;

  bool m_switch;
};

#endif 
