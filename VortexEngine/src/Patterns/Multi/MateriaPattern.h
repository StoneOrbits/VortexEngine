#ifndef MATERIA_PATTERN_H
#define MATERIA_PATTERN_H

#include "BlinkStepPattern.h"

#include "../../Time/Timings.h"

class MateriaPattern : public BlinkStepPattern
{
public:
  MateriaPattern(const PatternArgs& args);
  virtual ~MateriaPattern();

  // init the pattern to initial state
  virtual void init() override;

protected:
  virtual void blinkOn() override;
  virtual void poststep() override;

private:
  // how much the fill has progressed
  uint8_t m_progress;
};

#endif
