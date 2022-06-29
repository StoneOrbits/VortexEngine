#ifndef SEQUENCED_PATTERN_H
#define SEQUENCED_PATTERN_H

#include "HybridPattern.h"

#include "../../LedTypes.h"
#include "../../Timer.h"

class SequencedPattern;
class SingleLedPattern;
class SerialBuffer;

// a map of leds to pattern ids
class PatternMap
{
public:
  PatternMap();
  PatternMap(PatternID pattern, LedMap positions = MAP_LED_ALL);

  // set a pattern at each position in the LedMap
  void setPatternAt(PatternID pattern, LedMap positions);
  PatternID operator[](LedPos index) const;

  // serialize and unserialize a pattern map
  void serialize(SerialBuffer &buffer) const;
  void unserialize(SerialBuffer &buffer);

  // public list of pattern IDs for each led
  PatternID m_patternMap[LED_COUNT];
};

class ColorsetMap
{
public:
  ColorsetMap();
  ColorsetMap(const Colorset &colorset, LedMap positions = MAP_LED_ALL);

  // set a pattern at each position in the LedMap
  void setColorsetAt(const Colorset &colorset, LedMap positions);
  const Colorset &operator[](LedPos index) const;

  // serialize and unserialize a colorset map
  void serialize(SerialBuffer &buffer) const;
  void unserialize(SerialBuffer &buffer);

  // public list of pattern IDs for each led
  Colorset m_colorsetMap[LED_COUNT];
};

// Make an array of sequence steps to create a sequenced pattern
class SequenceStep
{
public:
  SequenceStep();
  SequenceStep(uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap = Colorset());
  SequenceStep(const SequenceStep &other);

  // serialize and unserialize a step in the sequencer
  void serialize(SerialBuffer &buffer) const;
  void unserialize(SerialBuffer &buffer);

  // public members to allow for easy initialization of an array of SequenceSteps
  uint16_t m_duration;
  PatternMap m_patternMap;
  ColorsetMap m_colorsetMap;
};

class SequencedPattern : public HybridPattern
{
public:
  // initialize a sequence pattern with a list of pointers to sequencesteps and the 
  // number of entries in that list
  SequencedPattern(uint32_t sequenceLength, const SequenceStep *sequence);
  virtual ~SequencedPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

protected:
  // static data
  uint32_t m_sequenceLength;
  SequenceStep *m_sequenceSteps;

  // runtime data
  uint32_t m_curSequence;
  Timer m_timer;
};

#endif
