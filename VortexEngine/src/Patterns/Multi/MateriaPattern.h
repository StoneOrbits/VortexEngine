#ifndef MATERIA_PATTERN_H
#define MATERIA_PATTERN_H

#include "MultiLedPattern.h"

#include "../../Time/Timings.h"
#include "../../Time/Timer.h"
#include "../../Leds/LedTypes.h"

class MateriaPattern : public MultiLedPattern
{
public:
  MateriaPattern(const PatternArgs &args);
  virtual ~MateriaPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

private:
  // blink durations
  uint8_t m_onDuration1;
  uint8_t m_offDuration1;
  uint8_t m_onDuration2;
  uint8_t m_offDuration2;
  // the speed for the step timer
  uint8_t m_stepSpeed;
  // the step timer
  Timer m_blinkTimer1;
  Timer m_blinkTimer2;
  Timer m_stepTimer;

  LedMap m_ledMap;

  bool m_switch;
};
#endif
