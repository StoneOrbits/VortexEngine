#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  {
    PATTERN_DOPS, 5, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE,
      0xABAA00,
      0x5500AB
    }
  }
};

// exposed size of the default modes array
const uint8_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));

// compile time assert for max modes count check
static_assert(!MAX_MODES || MAX_MODES == (sizeof(default_modes) / sizeof(default_modes[0])),
  "Incorrect number of default modes! Ensure default_modes array has "
  EXPAND_AND_QUOTE(MAX_MODES) " enties");
