#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  // Pattern 1: Jest mode
  PATTERN_JEST, {
    HSV(0, 255, 255),
    HSV(96, 255, 255),
    HSV(160, 255, 255),
    HSV(64, 255, 255),
    HSV(192, 255, 255)
  },

  // Pattern 2: Ghost Crush Mode
  PATTERN_GHOSTCRUSH, {
    HSV(0, 0, 255),
    HSV(0, 0, 255),
    HSV(0, 0, 0),
    HSV(0, 255, 170),
    HSV(0, 0, 0)
  },

  // Pattern 3: Impact
  PATTERN_IMPACT, {
    HSV(160, 255, 255),
    HSV(64, 255, 255),
    HSV(160, 255, 255),
    HSV(0, 255, 255),
    HSV(96, 255, 255),
    HSV(160, 255, 255),
    HSV(96, 255, 255),
    HSV(160, 255, 255)
  },

  // Pattern 4: Warp Worm
  PATTERN_WARPWORM, {
    HSV(96, 255, 255),
    HSV(192, 255, 170)
  },

  // Pattern 5: Pulsish
  PATTERN_PULSISH, {
    HSV(128, 255, 255),
    HSV(224, 170, 255),
    HSV(160, 255, 85)
  },

  // Pattern 6: ZigZag
  PATTERN_ZIGZAG, {
    HSV(80, 255, 0),
    HSV(80, 255, 255),
    HSV(192, 255, 255),
    HSV(0, 0, 0),
    HSV(0, 255, 255),
    HSV(0, 255, 170)
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

  // Pattern 8: Snowball
  PATTERN_SNOWBALL, {
    HSV(18, 255, 85),
    HSV(103, 255, 191),
    HSV(188, 255, 123)
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

  // Pattern 10: Materia
  PATTERN_MATERIA, {
    HSV(224, 255, 255),
    HSV(160, 85, 255),
    HSV(192, 255, 85),
    HSV(128, 170, 255)
  },

  // Pattern 11: VortexWipe
  PATTERN_VORTEXWIPE, {
    HSV(0, 255, 255),
    HSV(160, 255, 85),
    HSV(160, 255, 85),
    HSV(160, 255, 85),
    HSV(160, 255, 85),
    HSV(160, 255, 85),
    HSV(160, 255, 85),
    HSV(160, 255, 85)
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

  // Pattern 13: VortexWipe2
  PATTERN_VORTEXWIPE, {
    HSV(128, 255, 255),
    HSV(208, 255, 255),
    HSV(16, 170, 255)
  },

  // Pattern 14: Rabbit
  PATTERN_RABBIT, {
    HSV(160, 255, 255),
    HSV(0, 255, 255),
    HSV(96, 255, 255),
    HSV(160, 255, 255),
    HSV(96, 255, 255),
    HSV(160, 255, 255)
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
