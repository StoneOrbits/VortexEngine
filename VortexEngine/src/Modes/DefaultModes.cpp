#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  // Pattern 1: Jest mode
  {
    PATTERN_BACKSTROBE, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE,
      0xABAA00,
      0x5500AB
    }
  },

#if 0
  // Pattern 2: Ghost Crush Mode
  {
    PATTERN_GHOSTCRUSH, {
      RGB_WHITE,
      RGB_WHITE,
      RGB_OFF,
      0x700000,
      RGB_OFF,
    }
  },

  // Pattern 3: Impact
  {
    PATTERN_IMPACT, {
      RGB_BLUE,
      0xABAA00,
      RGB_BLUE,
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE,
      RGB_GREEN,
      RGB_BLUE
    }
  },

  // Pattern 4: Warp Worm
  {
    PATTERN_WARPWORM, {
      RGB_GREEN,
      0x26004B,
    }
  },

  // Pattern 5: Pulsish
  {
    PATTERN_PULSISH, {
      0x00AB55,
      0x8D1C55,
      0x00001C
    }
  },

  // Pattern 6: ZigZag
  {
    PATTERN_ZIGZAG, {
      RGB_OFF,
      0x56D400,
      0x5500AB,
      RGB_OFF,
      RGB_RED,
      0x700000
    }
  },

  // Pattern 7: Strobe
  {
    PATTERN_STROBE, {
      0xD4002B,
      RGB_OFF,
      0x0056AA,
      RGB_OFF,
      0x8E711C,
      RGB_OFF,
      0x0056AA,
      RGB_OFF
    }
  },

  // Pattern 8: Snowball
  {
    PATTERN_SNOWBALL, {
      0x170600,
      0x00840A,
      0x12002A
    }
  },

  // Pattern 9: Ultradops
  {
    PATTERN_ULTRADOPS, {
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

  // Pattern 10: Materia
  {
    PATTERN_MATERIA, {
      0xAA0055,
      0x7070C5,
      0x0A0013,
      0x1C8E55,
    }
  },

  // Pattern 11: VortexWipe
  {
    PATTERN_VORTEXWIPE, {
      RGB_RED,
      0x00001C,
      0x00001C,
      0x00001C,
      0x00001C,
      0x00001C,
      0x00001C,
      0x00001C,
    }
  },

  // Pattern 12: GhostCrush
  {
    PATTERN_GHOSTCRUSH, {
      0x26004B,
      RGB_OFF,
      RGB_GREEN,
      RGB_WHITE,
      RGB_GREEN,
      RGB_OFF,
      0x26004B,
    }
  },

  // Pattern 13: VortexWipe2
  {
    PATTERN_VORTEXWIPE, {
      0x00AB55,
      0x7F0081,
      0xAA381C,
    }
  },

  // Pattern 14: Rabbit
  {
    PATTERN_RABBIT, {
      RGB_BLUE,
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE,
      RGB_GREEN,
      RGB_BLUE,
    }
  },

  // Pattern 15: Complementary Blend
  {
    PATTERN_COMPLEMENTARY_BLEND, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE
    }
  }
#endif
};

// exposed size of the default modes array
const uint32_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));
