#ifndef DEFAULT_MODES_H
#define DEFAULT_MODES_H

#include "../Patterns/Patterns.h"
#include "../Leds/LedTypes.h"
#include "../VortexConfig.h"

// structure of the entries in the default modes array
struct DefaultLedEntry
{
  PatternID patternID;
  uint8_t numColors;
  const uint32_t *cols;
};

struct DefaultModeEntry
{
  DefaultLedEntry leds[LED_COUNT];
};

// the number of default modes, this is separate from MAX_MODES because the
// tests set MAX_MODES to 0 (unlimited) which would make the array empty
#define NUM_DEFAULT_MODES 9

// exposed global array of default modes
extern const DefaultModeEntry defaultModes[NUM_DEFAULT_MODES];

#endif
