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

// macros to create a PatternMap with a given Colorset and some preset LedMaps
#define oddTipsColorset(colorset) ColorsetMap(colorset, MAP_FINGER_ODD_TIPS)
#define oddTopsColorset(colorset) ColorsetMap(colorset, MAP_FINGER_ODD_TOPS)
#define evenTipsColorset(colorset) ColorsetMap(colorset, MAP_FINGER_EVEN_TIPS)
#define evenTopsColorset(colorset) ColorsetMap(colorset, MAP_FINGER_EVEN_TOPS)

#define RGB_SET Colorset(RGB_RED, RGB_GREEN, RGB_BLUE)

// define the pattern as 10 steps made up from the above 4 possible steps
SequenceStep theaterChaseSteps[] = {
  // ms   pattern                        colorset
  { 25,   oddTipsPattern(PATTERN_DOPS),  ColorsetMap() }, // step 1 RGB dops on odd tips for 25
  { 25,   oddTopsPattern(PATTERN_DOPS),  ColorsetMap() }, // step 2 RGB dops on odd tops for 25
  { 25,   oddTipsPattern(PATTERN_DOPS),  ColorsetMap() }, // step 3 RGB dops on odd tips for 25
  { 25,   oddTopsPattern(PATTERN_DOPS),  ColorsetMap() }, // step 4 RGB dops on odd tops for 25
  { 25,   oddTipsPattern(PATTERN_DOPS),  ColorsetMap() }, // step 5 RGB dops on odd tips for 25
  { 25,   evenTipsPattern(PATTERN_DOPS), ColorsetMap() }, // step 6 RGB dops on odd tips for 25
  { 25,   evenTopsPattern(PATTERN_DOPS), ColorsetMap() }, // step 7 RGB dops on odd tops for 25
  { 25,   evenTipsPattern(PATTERN_DOPS), ColorsetMap() }, // step 8 RGB dops on odd tips for 25
  { 25,   evenTopsPattern(PATTERN_DOPS), ColorsetMap() }, // step 9 RGB dops on odd tops for 25
  { 25,   evenTipsPattern(PATTERN_DOPS), ColorsetMap() }, // step 10 RGB dops on odd tips for 25
};

Pattern *createTheaterChase()
{
  static SequenceStep theaterChaseSteps[10];
  for (uint32_t i = 0; i < 10; ++i) {
    // all of the fingers are dops
    PatternMap patMap;
    switch ((i < 5) << 1 | ((i % 2) == 0)) {
    case 3: // i < 5 && i % 2 == 0
      patMap = oddTipsPattern(PATTERN_DOPS);
      break;
    case 2: // i < 5 && i % 2 != 0
      patMap = oddTopsPattern(PATTERN_DOPS);
      break;
    case 1:
      patMap = evenTipsPattern(PATTERN_DOPS);
      break;
    case 0:
      patMap = evenTopsPattern(PATTERN_DOPS);
      break;
    }
    // fill out this step of the theaterChaseSteps
    theaterChaseSteps[i] = SequenceStep(25, patMap, Colorset());
  }
  return new SequencedPattern(10, theaterChaseSteps);
}

Pattern *createChaser()
{
  static SequenceStep chaserSteps[8];
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
