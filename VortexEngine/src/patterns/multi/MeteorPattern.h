#ifndef METEOR_PATTERN_H
#define METEOR_PATTERN_H

#include "multiledpattern.h"

#include "../../Timer.h"
#include "../../LedStash.h"

class MeteorPattern : public MultiLedPattern
{
public:
  MeteorPattern();
  virtual ~MeteorPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer& buffer) const override;
  virtual void unserialize(SerialBuffer& buffer) override;

private:
  Timer m_blinkTimer;
  Timer m_spawnTimer;

  LedStash m_stash;
};
#endif
