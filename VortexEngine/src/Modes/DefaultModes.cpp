#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
  {
    PATTERN_BOUNCE, 3, {
      HSV(0, 255, 255),
      HSV(96, 255, 255),
      HSV(160, 255, 255)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(30, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(50, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(80, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(110, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(140, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(170, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(200, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(230, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_GHOSTCRUSH, 5, {
      HSV(0, 0, 255),
      HSV(0, 0, 255),
      HSV(0, 0, 0),
      HSV(0, 255, 170),
      HSV(0, 0, 0)
    }
  },

  {
    PATTERN_STROBE, 8, {
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
    PATTERN_GHOSTCRUSH, 7, {
      0x26004B,
      RGB_OFF,
      RGB_GREEN,
      RGB_WHITE,
      RGB_GREEN,
      RGB_OFF,
      0x26004B,
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
