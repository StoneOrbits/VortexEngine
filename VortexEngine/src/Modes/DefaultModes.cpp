#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  // Pattern 1: Jest mode
  {
    PATTERN_BLEND, {
      RGBColor(255, 0, 0),
      RGBColor(0, 255, 0),
      RGBColor(0, 0, 255),
    }
  },
};

// exposed size of the default modes array
const uint32_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));
