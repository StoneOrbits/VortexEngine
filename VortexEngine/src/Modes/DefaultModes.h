#ifndef DEFAULT_MODES_H
#define DEFAULT_MODES_H

#include "../Patterns/Patterns.h"
#include "../Leds/LedTypes.h"
#include "../VortexConfig.h"

struct DefaultLedEntry
{
  PatternID patternID;
  uint8_t numColors;
  const uint32_t *cols;
};

struct DefaultModeEntry
{
  struct DefaultLedEntry leds[LED_COUNT];
};

extern const struct DefaultModeEntry defaultModes[MAX_MODES];

#endif
