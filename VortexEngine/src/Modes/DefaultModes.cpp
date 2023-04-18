#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  {
    PATTERN_STROBE, 5, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE,
      0xABAA00,
      0x5500AB
    }
  },
  {
    PATTERN_GHOSTCRUSH, 5, {
      RGB_WHITE,
      RGB_WHITE,
      RGB_OFF,
      0x700000,
      RGB_OFF,
    }
  },
  {
    PATTERN_BRACKETS, 6, {
      RGB_OFF,
      0x56D400,
      0x5500AB,
      RGB_OFF,
      RGB_RED,
      0x700000
    }
  },
  {
    PATTERN_ULTRADOPS, 8, {
      0x1C0000,
      0x4B2600,
      0xABAA00,
      0x001C00,
      0x00130A,
      0x00001C,
      0x26004B,
      0x13000A
    }
  },
  {
    PATTERN_COMPLEMENTARY_BLEND, 3, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE
    }
  }
};

// exposed size of the default modes array
const uint8_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));
