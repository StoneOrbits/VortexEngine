#ifndef MATERIA_PATTERN_H
#define MATERIA_PATTERN_H

#include "MultiLedPattern.h"

#include "../../Time/Timer.h"
#include "../../Leds/LedTypes.h"

class MateriaPattern : public MultiLedPattern
{
public:
  MateriaPattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t onDuration2, uint8_t offDuration2, uint16_t stepSpeed = 800);
  virtual ~MateriaPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream& buffer) const override;
  virtual void unserialize(ByteStream& buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate(int level = 0) const override;
#endif

private:
  // blink durations
  uint8_t m_onDuration1;
  uint8_t m_offDuration1;
  uint8_t m_onDuration2;
  uint8_t m_offDuration2;
  // the speed for the step timer
  uint16_t m_stepSpeed;
  // the step timer
  Timer m_blinkTimer1;
  Timer m_blinkTimer2;
  Timer m_stepTimer;

  bool m_switch;

  LedMap m_ledMap;
};
#endif 
