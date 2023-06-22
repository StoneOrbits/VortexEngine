#include "ChaserPattern.h"

// This pattern aims to be a demonstration of the sequencer.
// There are always many ways to implement a pattern, it's best
// to choose the method that is most suitable for the pattern.
ChaserPattern::ChaserPattern(const PatternArgs &args) :
  SequencedPattern(args)
{
  setArgs(args);
  // set the pattern ID
  //m_patternID = PATTERN_CHASER;
  // There are 8 steps in the chaser, so iterate 8 times and generate
  // a pattern map for each step. A colorset map can also be applied
  // to override certain colors for specific steps, but that's not
  // what is being done here
  for (uint8_t i = 0; i < 8; ++i) {
    // Each step all fingers are dops except for one, so start with a
    // Pattern Map that has dops on all fingers. A Pattern Map will map
    // a Pattern ID to each LED on the device, then we will override a
    // different entry each step with the Pattern ID for Solid0.
    PatternMap patMap(PATTERN_DOPS);
    // Override a single finger in the pattern map with the Solid0 pattern
    // which will use the 0th color from the colorset as the solid color.
    // An LedMap is a bitmap that indicates which leds are turned on or off
    // at any given time. This will generate an Led Map based on the current
    // step index like this:
    //
    //  step -> finger index -> target leds -> LedMap
    // -----------------------------------------------------
    //     0         0             0, 1        00 00 00 00 11
    //     1         1             2, 3        00 00 00 11 00
    //     2         2             4, 5        00 00 11 00 00
    //     3         3             6, 7        00 11 00 00 00
    //     4         4             8, 9        11 00 00 00 00
    //     5         3             6, 7        00 11 00 00 00
    //     6         2             4, 5        00 00 11 00 00
    //     7         1             2, 3        00 00 00 11 00
    LedMap overrideLeds = MAP_PAIR((Pair)((i < 5) ? i : (8 - i)));
    // Then this API is used to override specific positions in the Pattern Map
    // with a different pattern ID, we use the Led Map generated above to tell
    // setPatternAt() which indices to override with Solid0
    patMap.setPatternAt(PATTERN_SOLID, overrideLeds);
    // Then finally we add this pattern mapping to the sequence in a new step
    // that will last 300ms, this means all 8 steps will be 300ms each.
    // The last parameter of addStep() is omitted, that parameter could be used
    // to override the colorset for specific Leds on any given step. Since it
    // is omitted that means this pattern will use whichever colorset is chosen
    m_sequence.addStep(300, patMap);
  }
}
