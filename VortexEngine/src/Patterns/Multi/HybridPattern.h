#ifndef HYBRID_PATTERN_H
#define HYBRID_PATTERN_H

#include "MultiLedPattern.h"

class SingleLedPattern;

// A hybrid pattern is a type of multi-led pattern that works by displaying
// pre-existing patterns on different leds simultaneously
class HybridPattern : public MultiLedPattern
{
protected:
  // HybridPattern is an abstract class
  HybridPattern();

public:
  virtual ~HybridPattern();

  // init the pattern to initial state
  virtual void init() override;

  // play the hybrid pattern (play all led patterns)
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream &buffer) const override;
  virtual void unserialize(ByteStream &buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate(int level = 0) const override;
#endif

protected:
  // set a pattern at an index of the hybrid pattern (optionally replace colorset)
  void setPatternAt(LedPos pos, SingleLedPattern *pat, const Colorset *set = nullptr);
  void clearPatterns();

  // array of single LED patterns, one for each LED
  SingleLedPattern *m_ledPatterns[LED_COUNT];
};

#endif
