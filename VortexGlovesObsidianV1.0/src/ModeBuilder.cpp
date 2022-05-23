#include "ModeBuilder.h"

#include "patterns/SingleLedPattern.h"
#include "patterns/MultiLedPattern.h"
#include "patterns/Pattern.h"
#include "PatternBuilder.h"
#include "Colorset.h"
#include "Mode.h"
#include "Log.h"

ModeBuilder::ModeBuilder()
{
}

// make with pattern and a copy of a colorset set
Mode *ModeBuilder::make(PatternID id, const Colorset *set)
{
  if (id > PATTERN_LAST) {
    return nullptr;
  }
  if (id > PATTERN_SINGLE_LAST) {
    return makeMulti(id, set);
  }
  return makeSingle(id, set);
}

// make a simple mode with a single pattern and up to 8 colors
Mode *ModeBuilder::make(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
  RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset set(c1, c2, c3, c4, c5, c6, c7, c8);  
  return make(id, &set);
}

Mode *ModeBuilder::makeSingle(PatternID id, const Colorset *set)
{
  // create the new mode object
  Mode *newMode = new Mode();
  if (!newMode) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // create a new colorset from the list of colors
    Colorset *newSet = new Colorset(*set);
    if (!newSet) {
      ERROR_OUT_OF_MEMORY();
      delete newMode;
      return nullptr;
    }
    // create a new pattern from the id
    SingleLedPattern *newPat = PatternBuilder::makeSingle(id);
    if (!newPat) {
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

Mode *ModeBuilder::makeMulti(PatternID id, const Colorset *set)
{
  // create the new mode object
  Mode *newMode = new Mode();
  if (!newMode) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  // create a new colorset from the list of colors
  Colorset *newSet = new Colorset(*set);
  if (!newSet) {
    ERROR_OUT_OF_MEMORY();
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
