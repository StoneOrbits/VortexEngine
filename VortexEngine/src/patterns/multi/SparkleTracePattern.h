#ifndef SPARKLETRACE_PATTERN_H
#define SPARKLETRACE_PATTERN_H

#include "multiledpattern.h"

#include "../../Timer.h"
#include "../../LedStash.h"

class SparkleTracePattern : public MultiLedPattern
{
public:
  SparkleTracePattern();
  virtual ~SparkleTracePattern();

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
};
#endif