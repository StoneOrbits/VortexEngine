#include "ChaserPattern.h"

// This controls the ratio of chaser dots to LED_COUNT. Default 1 chaser per 7 LEDs. Range: 1-LED_COUNT.
#define CHASER_RATIO 7


// This pattern aims to be a demonstration of the sequencer.
// There are always many ways to implement a pattern, it's best
// to choose the method that is most suitable for the pattern.
ChaserPattern::ChaserPattern(const PatternArgs &args) :
  SequencedPattern(args)
{
  setArgs(args);
  
  // Makes sure there is at least 1 chaser
  uint32_t numChasers = LED_COUNT / CHASER_RATIO;
  if (!numChasers) {
    numChasers = 1;
  }
  // set the pattern ID
  //m_patternID = PATTERN_CHASER;
  // There are LED_COUNT steps in the chaser, so iterate LED_COUNT times and generate
  // a pattern map for each step. A colorset map can also be applied
  // to override certain colors for specific steps, but that's not
  // what is being done here
  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    // Each step all LEDs are dops except for one, so start with a
    // Pattern Map that has dops on all LEDs. A Pattern Map will map
    // a Pattern ID to each LED on the device, then we will override a
    // different entry each step with the Pattern ID for Solid0.
    PatternMap patMap(PATTERN_DOPS);
    // Override a single LED in the pattern map with the Solid0 pattern
    // which will use the 0th color from the colorset as the solid color.
    // An LedMap is a bitmap that indicates which leds are turned on or off
    // at any given time. This will generate an Led Map based on the current
    LedMap overrideLeds = MAP_LED_NONE; 
    // This creates an led map with 1 chaser per CHASER_RATIO (default 7) leds in LED_COUNT
    for (uint8_t chaserCount = 0; chaserCount < numChasers; ++chaserCount) {
      overrideLeds |= MAP_LED((i + (chaserCount * CHASER_RATIO)) % LED_COUNT);
    }
    // Then this API is used to override specific positions in the Pattern Map
    // with a different pattern ID, we use the Led Map generated above to tell
    // setPatternAt() which indices to override with Solid0
    patMap.setPatternAt(PATTERN_SOLID, overrideLeds);
    // Then finally we add this pattern mapping to the sequence in a new step
    // that will last 300ms, this means all LED_COUNT steps will be 300ms each.
    // The last parameter of addStep() is omitted, that parameter could be used
    // to override the colorset for specific Leds on any given step. Since it
    // is omitted that means this pattern will use whichever colorset is chosen
    m_sequence.addStep(300, patMap);
  }
}
