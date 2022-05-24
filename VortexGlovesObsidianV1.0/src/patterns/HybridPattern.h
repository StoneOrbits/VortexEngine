#ifndef HYBRID_PATTERN_H
#define HYBRID_PATTERN_H

#include "MultiLedPattern.h"

class SingleLedPattern;

// A hybrid pattern is a type of multi-led pattern that works by displaying 
// pre-existing patterns on different leds simultaneously
class HybridPattern : public MultiLedPattern
{
public:
  HybridPattern();
  virtual ~HybridPattern();

  // init the pattern to initial state
  virtual void init();

  // play the hybrid pattern (play all led patterns)
  virtual void play();

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer &buffer) const;
  virtual void unserialize(SerialBuffer &buffer);

protected:
  // array of single LED patterns, one for each LED
  SingleLedPattern *m_ledPatterns[LED_COUNT];
};

#endif
