#include "ModeBuilder.h"

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
    Pattern *newPat = PatternBuilder::make(id);
    if (!newPat) {
      // allocation error
      delete newMode;
      delete newSet;
      return nullptr;
    }
    // bind the pattern and colorset to the mode
    if (!newMode->bind(newPat, newSet, pos)) {
      delete newSet;
      delete newPat;
      delete newMode;
      return nullptr;
    }
  }
  return newMode;
}
