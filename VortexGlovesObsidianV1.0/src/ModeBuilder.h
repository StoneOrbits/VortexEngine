#ifndef MODE_BUILDER_H
#define MODE_BUILDER_H

#include "ColorTypes.h"

class Colorset;
class Pattern;
class Mode;

// list of patterns that can be built
enum PatternID : uint32_t
{
  PATTERN_FIRST,

  PATTERN_STROBE = PATTERN_FIRST,  // basicpattern 5 8
  PATTERN_HYPERSTROBE, // basicpattern 25 25
  PATTERN_DOPS, // basicpattern 2 13
  PATTERN_DOPISH, // basicpattern 2 7
  PATTERN_ULTRADOPS, // basicpattern 1 3
  PATTERN_STROBIE, // basicpattern 3 22

  PATTERN_COUNT, // total number of patterns
  PATTERN_LAST = (PATTERN_COUNT - 1)
};

class ModeBuilder
{
    // private constructor static class
    ModeBuilder();

  public:
    // make simple mode with a single pattern and 1-8 colors
    static Mode *make(PatternID id, RGBColor c1, RGBColor c2 = RGB_OFF, 
        RGBColor c3 = RGB_OFF, RGBColor c4 = RGB_OFF, RGBColor c5 = RGB_OFF,
        RGBColor c6 = RGB_OFF, RGBColor c7 = RGB_OFF, RGBColor c8 = RGB_OFF);

  private:
    // helper to build a pattern from ID
    static Pattern *makePattern(PatternID id);
};

#endif
