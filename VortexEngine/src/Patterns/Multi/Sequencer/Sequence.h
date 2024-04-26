#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "../../Patterns.h"
#include "../../../Leds/LedTypes.h"
#include "../../../Colors/Colorset.h"
#include "../../../Time/Timer.h"

class SequencedPattern;
class SingleLedPattern;
class ByteStream;

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
  bool serialize(ByteStream &buffer) const;
  bool unserialize(ByteStream &buffer);

  // public list of pattern IDs for each led
  PatternID m_patternMap[LED_COUNT];
};

// A map of leds to colorsets
class ColorsetMap
{
public:
  ColorsetMap();
  ColorsetMap(const Colorset &colorset, LedMap positions = MAP_LED_ALL);

  // set a pattern at each position in the LedMap
  void setColorsetAt(const Colorset &colorset, LedMap positions);
  const Colorset &operator[](LedPos index) const;

  // serialize and unserialize a colorset map
  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

  // public list of pattern IDs for each led
  Colorset m_colorsetMap[LED_COUNT];
};

// A single step in a sequence
class SequenceStep
{
public:
  SequenceStep();
  SequenceStep(uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap = Colorset());
  SequenceStep(const SequenceStep &other);

  // serialize and unserialize a step in the sequencer
  bool serialize(ByteStream &buffer) const;
  bool unserialize(ByteStream &buffer);

  // public members to allow for easy initialization of an array of SequenceSteps
  uint16_t m_duration;
  PatternMap m_patternMap;
  ColorsetMap m_colorsetMap;
};

// A sequence which can contain multiple SequenceSteps
class Sequence
{
public:
  Sequence();
  ~Sequence();

  Sequence(const Sequence &other);
  Sequence(Sequence &&other) noexcept;

  void operator=(const Sequence &other);
  bool operator==(const Sequence &other) const;
  bool operator!=(const Sequence &other) const;

  void initSteps(uint8_t numSteps);
  uint8_t addStep(const SequenceStep &step);
  uint8_t addStep(uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap = Colorset());
  void clear();

  bool serialize(ByteStream &buffer) const;
  bool unserialize(ByteStream &buffer);

  uint8_t numSteps() const;
  const SequenceStep &operator[](uint8_t index) const;

private:
  // static data
  SequenceStep *m_sequenceSteps;
  uint8_t m_numSteps;
};

#endif
