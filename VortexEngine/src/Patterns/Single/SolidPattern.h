#ifndef SOLID_PATTERN_H
#define SOLID_PATTERN_H

#include "BasicPattern.h"

class SolidPattern : public BasicPattern
{
public:
  SolidPattern(uint8_t colIndex, uint8_t onDuration, uint8_t offDuration = 0, uint8_t gapDuration = 0);
  virtual ~SolidPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(ByteStream &buffer) const override;
  virtual void unserialize(ByteStream &buffer) override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

protected:
  // callbacks for blinking on/off, can be overridden by derived classes
  virtual void onBlinkOn();

  uint8_t m_colIndex;
};

#endif
