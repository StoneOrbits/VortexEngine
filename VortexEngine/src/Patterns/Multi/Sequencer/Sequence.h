#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "../../Patterns.h"
#include "../../../Leds/LedTypes.h"
#include "../../../Colors/Colorset.h"
#include "../../../Time/Timer.h"

#if FIXED_LED_COUNT == 0
#include <vector>
#endif

class SequencedPattern;
class SingleLedPattern;
class ByteStream;

// a map of leds to pattern ids
class PatternMap
{
public:
  PatternMap(VortexEngine &engine);
  PatternMap(VortexEngine &engine, PatternID pattern, LedMap positions);

  // set a pattern at each position in the LedMap
  void setPatternAt(PatternID pattern, LedMap positions);
  PatternID operator[](LedPos index) const;

  // serialize and unserialize a pattern map
  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

  // engine reference
  VortexEngine &m_engine;

#if FIXED_LED_COUNT == 0
  // public list of pattern IDs for each led
  std::vector<PatternID> m_patternMap;
#else
  PatternID m_patternMap[LED_COUNT];
#endif
};

// A map of leds to colorsets
class ColorsetMap
{
public:
  ColorsetMap(VortexEngine &engine);
  ColorsetMap(VortexEngine &engine, const Colorset &colorset, LedMap positions);

  // set a pattern at each position in the LedMap
  void setColorsetAt(const Colorset &colorset, LedMap positions);
  const Colorset &operator[](LedPos index) const;

  // serialize and unserialize a colorset map
  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

  // engine reference
  VortexEngine &m_engine;

#if FIXED_LED_COUNT == 0
  // public list of pattern IDs for each led
  std::vector<Colorset> m_colorsetMap;
#else
  Colorset m_colorsetMap[LED_COUNT];
#endif
};

// A single step in a sequence
class SequenceStep
{
public:
  SequenceStep(VortexEngine &engine);
  SequenceStep(VortexEngine &engine, uint16_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap);
  SequenceStep(const SequenceStep &other);

  // serialize and unserialize a step in the sequencer
  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

  // engine reference
  VortexEngine &m_engine;

  // public members to allow for easy initialization of an array of SequenceSteps
  uint16_t m_duration;
  PatternMap m_patternMap;
  ColorsetMap m_colorsetMap;
};

// A sequence which can contain multiple SequenceSteps
class Sequence
{
public:
  Sequence(VortexEngine &engine);
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

  void serialize(ByteStream &buffer) const;
  void unserialize(ByteStream &buffer);

  uint8_t numSteps() const;
  const SequenceStep &operator[](uint8_t index) const;

private:
  // engine reference
  VortexEngine &m_engine;

  // static data
  SequenceStep *m_sequenceSteps;
  uint8_t m_numSteps;
};

#endif
