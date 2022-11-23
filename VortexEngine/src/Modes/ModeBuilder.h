#ifndef MODE_BUILDER_H
#define MODE_BUILDER_H

#include "../Colors/ColorTypes.h"
#include "../Patterns/Patterns.h"

class ByteStream;
class Colorset;
class Pattern;
class Mode;

class ModeBuilder
{
  // private constructor static class
  ModeBuilder();

public:
  // make with pattern and a copy of a colorset set
  static Mode *make(PatternID id, const Colorset *set);
  // make simple mode with a single pattern and 1-8 colors
  static Mode *make(PatternID id, RGBColor c1, RGBColor c2 = RGB_OFF,
    RGBColor c3 = RGB_OFF, RGBColor c4 = RGB_OFF, RGBColor c5 = RGB_OFF,
    RGBColor c6 = RGB_OFF, RGBColor c7 = RGB_OFF, RGBColor c8 = RGB_OFF);

  // unserialize a buffer into a mode
  static Mode *unserialize(ByteStream &buffer);

private:
};

#endif
