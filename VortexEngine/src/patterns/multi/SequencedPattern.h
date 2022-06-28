#ifndef SEQUENCED_PATTERN_H
#define SEQUENCED_PATTERN_H

#include "MultiLedPattern.h"

#include "../../LedConfig.h"
#include "../../Timer.h"

class SequencedPattern;
class SingleLedPattern;

// a map of leds to pattern ids
class PatternMap
{
public:
  PatternMap() :
    m_patternMap()
  {
    for (uint32_t i = 0; i < LED_COUNT; ++i) {
      m_patternMap[i] = PATTERN_NONE;
    }
  }
  PatternMap(PatternID pattern, LedMap positions = MAP_FINGER_ALL) :
    PatternMap()
  {
    setPatternAt(pattern, positions);
  }

  // set a pattern at each position in the LedMap
  void setPatternAt(PatternID pattern, LedMap positions) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      if (checkLed(positions, pos)) {
        m_patternMap[pos] = pattern;
      }
    }
  }
  PatternID operator[](LedPos index) const { 
    return m_patternMap[index]; 
  }
  // a list of pattern IDs for each led
  PatternID m_patternMap[LED_COUNT];
};

class ColorsetMap
{
public:
  ColorsetMap() :
    m_colorsetMap()
  {
  }
  ColorsetMap(const Colorset &colorset, LedMap positions = MAP_FINGER_ALL) :
    ColorsetMap()
  {
    setColorsetAt(colorset, positions);
  }

  // set a pattern at each position in the LedMap
  void setColorsetAt(const Colorset &colorset, LedMap positions) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      if (checkLed(positions, pos)) {
        m_colorsetMap[pos] = colorset;
      }
    }
  }
  const Colorset &operator[](LedPos index) const { 
    return m_colorsetMap[index]; 
  }
  // a list of pattern IDs for each led
  Colorset m_colorsetMap[LED_COUNT];
};

// Make an array of sequence steps to create a sequenced pattern
class SequenceStep
{
public:
  SequenceStep() :
    m_duration(0), m_patternMap(), m_colorsetMap()
  {
  }
  SequenceStep(uint8_t duration, const PatternMap &patternMap, const ColorsetMap &colorsetMap) :
    m_duration(duration), m_patternMap(patternMap), m_colorsetMap(colorsetMap)
  {
  }
  SequenceStep(const SequenceStep &other) :
    SequenceStep(other.m_duration, other.m_patternMap, other.m_colorsetMap)
  {
  }
  // public members
  uint8_t m_duration;
  PatternMap m_patternMap;
  ColorsetMap m_colorsetMap;
};

class SequencedPattern : public MultiLedPattern
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

private:
  // static data
  uint32_t m_sequenceLength;
  const SequenceStep *m_sequenceSteps;

  // runtime data
  uint32_t m_curSequence;
  Timer m_timer;
  SingleLedPattern *m_curPatterns[LED_COUNT];
};

#endif
