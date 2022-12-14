#ifndef DASHDOPS_PATTERN_H
#define DASHSOPS_PATTERN_H

#include "SingleLedPattern.h"

#include "../../Time/Timer.h"

class DashDopsPattern : public SingleLedPattern
{
public:
  DashDopsPattern(uint8_t dashLength = 20, uint8_t dotLength = 1, uint8_t offDuration = 5);
  DashDopsPattern(const PatternArgs &args);
  virtual ~DashDopsPattern();

  virtual void init() override;

  virtual void play() override;

  virtual void serialize(ByteStream& buffer) const override;
  virtual void unserialize(ByteStream& buffer) override;

  virtual void setArgs(const PatternArgs &args) override;
  virtual void getArgs(PatternArgs &args) const override;

#if SAVE_TEMPLATE == 1
  virtual void saveTemplate(int level = 0) const override;
#endif

private:
  // the duration the light is on/off for
  uint8_t m_dashDuration;
  uint8_t m_dotDuration;
  uint8_t m_offDuration;
  // the timer for performing blinks
  Timer m_blinkTimer;
  // the counter for dot color
  uint8_t m_dotColor;
};

#endif
