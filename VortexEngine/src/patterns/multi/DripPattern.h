#ifndef DRIP_PATTERN_H
#define DRIP_PATTERN_H

#include "multiledpattern.h"

#include "../../Timer.h"

class DripPattern : public MultiLedPattern
{
public:
  DripPattern();
  virtual ~DripPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  Timer m_blinkTimer;
  Timer m_dripTimer;
  bool m_sync;
};
#endif
