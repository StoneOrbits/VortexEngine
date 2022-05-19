#include "ModeBuilder.h"

#include "patterns/SingleLedPattern.h"
#include "patterns/MultiLedPattern.h"
#include "patterns/Pattern.h"
#include "PatternBuilder.h"
#include "Colorset.h"
#include "Mode.h"

ModeBuilder::ModeBuilder()
{
}

// make a simple mode with a single pattern and up to 8 colors
Mode *ModeBuilder::make(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
  RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  if (id == PATTERN_NONE || id > PATTERN_MULTI_LAST) {
    return nullptr;
  }
  if (id > PATTERN_SINGLE_LAST) {
    return makeMulti(id, c1, c2, c3, c4, c5, c6, c7, c8);
  }
  return makeSingle(id, c1, c2, c3, c4, c5, c6, c7, c8);
}

Mode *ModeBuilder::makeSingle(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
  RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  // create the new mode object
  Mode *newMode = new Mode();
  if (!newMode) {
    // allocation error
    return nullptr;
  }
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // create a new colorset from the list of colors
    Colorset *newSet = new Colorset(c1, c2, c3, c4, c5, c6, c7, c8);
    if (!newSet) {
      // allocation error
      delete newMode;
      return nullptr;
    }
    // create a new pattern from the id
    SingleLedPattern *newPat = PatternBuilder::makeSingle(id);
    if (!newPat) {
      // allocation error
      delete newMode;
      delete newSet;
      return nullptr;
    }
    // bind the pattern and colorset to the mode
    if (!newMode->bindSingle(newPat, newSet, pos)) {
      delete newSet;
      delete newPat;
      delete newMode;
      return nullptr;
    }
  }
  return newMode;
}

Mode *ModeBuilder::makeMulti(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
  RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  // create the new mode object
  Mode *newMode = new Mode();
  if (!newMode) {
    // allocation error
    return nullptr;
  }
  // create a new colorset from the list of colors
  Colorset *newSet = new Colorset(c1, c2, c3, c4, c5, c6, c7, c8);
  if (!newSet) {
    // allocation error
    delete newMode;
    return nullptr;
  }
  // create a new pattern from the id
  MultiLedPattern *newPat = PatternBuilder::makeMulti(id);
  if (!newPat) {
    // allocation error
    delete newMode;
    delete newSet;
    return nullptr;
  }
  // bind the pattern and colorset to the mode
  if (!newMode->bindMulti(newPat, newSet)) {
    delete newSet;
    delete newPat;
    delete newMode;
    return nullptr;
  }
  return newMode;
}
