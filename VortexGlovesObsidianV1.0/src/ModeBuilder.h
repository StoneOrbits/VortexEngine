#ifndef MODE_BUILDER_H
#define MODE_BUILDER_H

#include "ColorTypes.h"
#include "Patterns.h"

class Colorset;
class Pattern;
class Mode;

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
  // helpers to build single/multi led patterns
  static Mode *makeSingle(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
    RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
  static Mode *makeMulti(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
    RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);

  // helper to build a pattern from ID
  static Pattern *makePattern(PatternID id);
};


#endif
