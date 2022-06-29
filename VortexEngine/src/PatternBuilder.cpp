#include "PatternBuilder.h"

#include "SerialBuffer.h"
#include "TimeControl.h"

#include "patterns/multi/TheaterChasePattern.h"
#include "patterns/multi/SequencedPattern.h"
#include "patterns/multi/HueShiftPattern.h"
#include "patterns/multi/RabbitPattern.h"

#include "patterns/single/ReciprocalBlendPattern.h"
#include "patterns/single/BracketsPattern.h"
#include "patterns/single/AdvancedPattern.h"
#include "patterns/single/TracerPattern.h"
#include "patterns/single/BasicPattern.h"
#include "patterns/single/BlendPattern.h"

#include "Log.h"

Pattern *PatternBuilder::make(PatternID id)
{
  if (id > PATTERN_LAST) {
    DEBUG_LOGF("Invalid pattern id: %u", id);
    return nullptr;
  }
  if (isMultiLedPatternID(id)) {
    return makeMulti(id);
  }
  return makeSingle(id);
}

// generate a single LED pattern (nullptr if patternid is not single LED)
SingleLedPattern *PatternBuilder::makeSingle(PatternID id)
{
  if (!isSingleLedPatternID(id)) {
    return nullptr;
  }
  Pattern *pat = makeInternal(id);
  if (!pat) {
    return nullptr;
  }
  // don't set any flags on single pattersn
  return (SingleLedPattern *)pat;
}

// generate a multi LED pattern (nullptr if patternid is not multi LED)
MultiLedPattern *PatternBuilder::makeMulti(PatternID id)
{
  if (!isMultiLedPatternID(id)) {
    return nullptr;
  }
  Pattern *pat = makeInternal(id);
  if (!pat) {
    return nullptr;
  }
  return (MultiLedPattern *)pat;
}

Pattern *PatternBuilder::unserialize(SerialBuffer &buffer)
{
  Pattern *pat = make((PatternID)buffer.unserialize8());
  if (!pat) {
    return nullptr;
  }
  pat->unserialize(buffer);
  return pat;
}

Pattern *PatternBuilder::makeInternal(PatternID id)
{
  Pattern *pat = generate(id);
  if (!pat) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  // set private pattern ID via friend class relationship
  pat->m_patternID = id;
  return pat;
}

// macros to create a PatternMap with a given PatternID and some preset LedMaps
#define oddTipsPattern(pattern) PatternMap(pattern, MAP_FINGER_ODD_TIPS)
#define oddTopsPattern(pattern) PatternMap(pattern, MAP_FINGER_ODD_TOPS)
#define evenTipsPattern(pattern) PatternMap(pattern, MAP_FINGER_EVEN_TIPS)
#define evenTopsPattern(pattern) PatternMap(pattern, MAP_FINGER_EVEN_TOPS)

// theater chase is 5x 25ms odd tips/tops alternating
// followed by 5x 25ms even tips/tops alternating
Pattern *createTheaterChase()
{
  SequenceStep theaterChaseSteps[10];
  PatternMap patMap;
  for (uint32_t i = 0; i < 10; ++i) {
    if (i < 5) {
      patMap = (i % 2) ? oddTopsPattern(PATTERN_DOPS) : oddTipsPattern(PATTERN_DOPS);
    } else {
      patMap = (i % 2) ? evenTopsPattern(PATTERN_DOPS) : evenTipsPattern(PATTERN_DOPS);
    }
    // add the pattern mapping with default colorset
    theaterChaseSteps[i] = SequenceStep(25, patMap);
  }
  return new SequencedPattern(10, theaterChaseSteps);
}

Pattern *createChaser()
{
  SequenceStep chaserSteps[8];
  for (uint32_t i = 0; i < 8; ++i) {
    // all of the fingers are dops
    PatternMap patMap(PATTERN_DOPS);
    // all of the fingers are default colorset
    ColorsetMap colMap;
    // there is one finger mapping that goes back and forth
    Finger finger = (Finger)((i < 5) ? i : (8 - i));
    // set the pattern = ribbon and colorset = red for that one finger
    patMap.setPatternAt(PATTERN_RIBBON, MAP_FINGER(finger));
    colMap.setColorsetAt(Colorset(RGB_RED), MAP_FINGER(finger));
    // fill out this step of the chaserSteps
    chaserSteps[i] = SequenceStep(300, patMap, colMap);
  }
  return new SequencedPattern(8, chaserSteps);
}

Pattern *PatternBuilder::generate(PatternID id)
{
  // NOTE: The timings of patterns are only defined at creation time
  //       which means altering the tickrate will not change how fast
  //       a pattern displays unless you re-create it
  switch (id) {
    case PATTERN_STROBE: return new BasicPattern(5, 8);
    case PATTERN_HYPERSTROBE: return new BasicPattern(25, 25);
    case PATTERN_DOPS: return new BasicPattern(2, 13);
    case PATTERN_DOPISH: return new BasicPattern(2, 7);
    case PATTERN_ULTRADOPS: return new BasicPattern(1, 3);
    case PATTERN_STROBIE: return new BasicPattern(3, 22);
    case PATTERN_RIBBON: return new BasicPattern(20);
  #if 0
    case PATTERN_MINIRIBBON: return new BasicPattern(3);
  #endif
    case PATTERN_TRACER: return new TracerPattern();
    case PATTERN_BLINKIE: return new BasicPattern(5, 8, 35);
    case PATTERN_GHOSTCRUSH: return new BasicPattern(1, 0, 50);
    case PATTERN_ADVANCED: return new AdvancedPattern(5, 5, 10, 2, 2, 1);
    case PATTERN_BLEND: return new BlendPattern();
    case PATTERN_RECIPROCAL_BLEND: return new ReciprocalBlendPattern();
    case PATTERN_BRACKETS: return new BracketsPattern();
    case PATTERN_RABBIT: return new RabbitPattern();
    case PATTERN_HUESHIFT: return new HueShiftPattern();
    case PATTERN_THEATER_CHASE: return createTheaterChase();
    case PATTERN_CHASER: return createChaser();
    default: break;
  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
