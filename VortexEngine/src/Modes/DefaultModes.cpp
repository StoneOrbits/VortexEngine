#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  // Pattern 2: Ghost Crush Mode
  PATTERN_RABBIT, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(0, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(30, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(50, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(80, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(110, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(140, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(170, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(200, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(230, 255, 170),
    HSV(0, 0, 0)
  },

    PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(0, 255, 170),
    HSV(0, 0, 0)
  },

  // Pattern 7: Strobe
  PATTERN_STROBE, {
    HSV(240, 255, 255),
    HSV(0, 0, 0),
    HSV(144, 255, 255),
    HSV(0, 0, 0),
    HSV(48, 170, 255),
    HSV(0, 0, 0),
    HSV(144, 255, 255),
    HSV(0, 0, 0)
  },

  // Pattern 9: Ultradops
  PATTERN_ULTRADOPS, {
    HSV(0, 255, 85),
    HSV(32, 255, 170),
    HSV(64, 255, 255),
    HSV(96, 255, 85),
    HSV(128, 255, 85),
    HSV(160, 255, 85),
    HSV(192, 255, 170),
    HSV(224, 255, 85)
  },

  // Pattern 12: GhostCrush
  PATTERN_GHOSTCRUSH, {
    HSV(192, 255, 170),
    HSV(0, 0, 0),
    HSV(96, 255, 255),
    HSV(96, 0, 255),
    HSV(96, 255, 255),
    HSV(0, 0, 0),
    HSV(192, 255, 170)
  },

  // Pattern 15: Complementary Blend
  PATTERN_COMPLEMENTARY_BLEND, {
    RGB_RED,
    RGB_GREEN,
    RGB_BLUE
  }
};

// exposed size of the default modes array
const uint32_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));
