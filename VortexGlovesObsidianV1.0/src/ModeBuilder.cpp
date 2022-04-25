#include "ModeBuilder.h"

#include "patterns/BasicPattern.h"

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
  // create a new colorset from the list of colors
  Colorset *newSet = new Colorset(c1, c2, c3, c4, c5, c6, c7, c8);
  if (!newSet) {
    // allocation error
    delete newMode;
    return nullptr;
  }
  // create a new pattern from the id
  Pattern *newPat = makePattern(id);
  if (!newPat) {
    // allocation error
    delete newMode;
    delete newSet;
    return nullptr;
  }
  // bind the pattern and colorset to the mode
  if (!newMode->bindAll(newPat, newSet)) {
    delete newSet;
    delete newPat;
    delete newMode;
    return nullptr;
  }
  return newMode;
}

Pattern *ModeBuilder::makePattern(PatternID id)
{
  switch (id) {
  default:
  case PATTERN_STROBE:      
    return new BasicPattern(5, 8);
  case PATTERN_HYPERSTROBE: 
    return new BasicPattern(25, 25);
  case PATTERN_DOPS:        
    return new BasicPattern(2, 13);
  case PATTERN_DOPISH:      
    return new BasicPattern(2, 7);
  case PATTERN_ULTRADOPS:   
    return new BasicPattern(1, 3);
  case PATTERN_STROBIE:     
    return new BasicPattern(3, 22);
  case PATTERN_RIBBON:
    return new BasicPattern(20);
  case PATTERN_MINIRIBBON:
    return new BasicPattern(3);
  }
}
