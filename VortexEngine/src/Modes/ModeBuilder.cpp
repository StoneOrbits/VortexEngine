#include "ModeBuilder.h"

#include "../Patterns/single/SingleLedPattern.h"
#include "../Patterns/multi/MultiLedPattern.h"
#include "../Patterns/Pattern.h"
#include "../Patterns/PatternBuilder.h"
#include "../Serial/ByteStream.h"
#include "../Colors/Colorset.h"
#include "../Modes/Mode.h"
#include "../Log/Log.h"

ModeBuilder::ModeBuilder()
{
}

// make with pattern and a copy of a colorset set
Mode *ModeBuilder::make(PatternID id, const Colorset *set)
{
  if (id > PATTERN_LAST) {
    return nullptr;
  }
  // create the new mode object
  Mode *newMode = new Mode();
  if (!newMode) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  // bind the pattern and colorset to the mode
  if (!newMode->setPattern(id, set)) {
    delete newMode;
    return nullptr;
  }
  return newMode;
}

// make a simple mode with a single pattern and up to 8 colors
Mode *ModeBuilder::make(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
  RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset set(c1, c2, c3, c4, c5, c6, c7, c8);
  return make(id, &set);
}

Mode *ModeBuilder::unserialize(ByteStream &buffer)
{
  // create the new mode object
  Mode *newMode = new Mode();
  if (!newMode) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  newMode->unserialize(buffer);
  return newMode;
}
