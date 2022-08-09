#ifndef CROSSDOPS_PATTERN_H
#define CROSSDOPS_PATTERN_H

#include "multiledpattern.h"

#include "../../Timer.h"
#include "../../LedTypes.h"

class CrossDopsPattern : public MultiLedPattern
{
public:
  CrossDopsPattern();
  virtual ~CrossDopsPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  Timer m_blinkTimer;
  Timer m_flipTimer;
  LedMap m_ledMap;
};
#endif
