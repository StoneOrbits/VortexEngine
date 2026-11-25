#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const default_mode_entry default_modes[] = {
#if POV_TEST_MODES == 1
  // POV default modes (see POV_TEST_MODES in VortexConfig.h): the native
  // american, greek and tribal inspired textures, each with a colorset from
  // the randomizer's colorset generator
  {
    PATTERN_POV_FEATHERS, 4, {
      0x003F11,
      0x00137E,
      0xBD008C,
      0xFCDB00
    }
  },
  {
    PATTERN_POV_SPIRALS, 5, {
      0x000FFF,
      0x550041,
      0xFF1E00,
      0x555200,
      0x00FF42
    }
  },
  {
    PATTERN_POV_LIGHTNING, 5, {
      0xFF008D,
      0x545300,
      0x45FF00,
      0x00AA50,
      0x000355
    }
  },
  {
    PATTERN_POV_TEETH, 5, {
      0x00C9FF,
      0xD800FF,
      0xFF007B,
      0xFF3C00,
      0x21FF00
    }
  },
  {
    PATTERN_POV_COLUMNS, 8, {
      0xFF9C00,
      0xE39418,
      0xC78B2B,
      0xAB7E38,
      0x8F6F3E,
      0x735E3E,
      0x574B39,
      0x3B352D
    }
  },
  {
    PATTERN_POV_ARROWS, 5, {
      0xBA00FF,
      0x553200,
      0xD5FF00,
      0x155500,
      0x006FFF
    }
  },
  {
    PATTERN_POV_DAZZLER, 8, {
      0x0062AA,
      0x093555,
      0x123955,
      0x1B3C54,
      0x6FC2FF,
      0x2E4455,
      0x374855,
      0x404B54
    }
  },
  {
    PATTERN_POV_DENTILS, 5, {
      0x3F1300,
      0x3F1300,
      0x0D7E00,
      0x0064BD,
      0xFC00F0
    }
  },
  {
    PATTERN_POV_KENTE, 8, {
      0x00C9FF,
      0x0039FF,
      0x5400FF,
      0xE400FF,
      0xFF008D,
      0xFF0C00,
      0xFF9C00,
      0xD5FF00
    }
  },
  {
    PATTERN_POV_NAVAJO, 7, {
      0x551000,
      0xAA2000,
      0x00EDFF,
      0x550000,
      0x00EDFF,
      0xAA2000,
      0x551000
    }
  },
  {
    PATTERN_POV_LAUREL, 8, {
      0x00E1FF,
      0x002555,
      0x0000FF,
      0x250054,
      0xE400FF,
      0x550039,
      0xFF0039,
      0xFF4200
    }
  },
  {
    PATTERN_POV_TATTOO, 3, {
      0x809DFF,
      0x0F1955,
      0xFF7692
    }
  },
  {
    PATTERN_POV_MUDCLOTH, 8, {
      0x00FEFF,
      0x005455,
      0x0087FF,
      0x000555,
      0x6600FF,
      0x4A0055,
      0xFF00AB,
      0x550011
    }
  },
  {
    PATTERN_POV_SHIELD, 6, {
      0x550005,
      0x550005,
      0xAA5C00,
      0xAA5C00,
      0xE7FF00,
      0xE7FF00
    }
  },
  {
    PATTERN_POV_EGGDART, 3, {
      0x6DF1FF,
      0x7FAA66,
      0x554950
    }
  },
  {
    PATTERN_POV_GREEKKEY, 3, {
      0x54000F,
      0xAA001E,
      0x001355
    }
  },
#else
  {
    PATTERN_DOPS, 5, {
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
    PATTERN_WARPWORM, 2, {
      RGB_GREEN,
      0x26004B,
    }
  },
  {
    PATTERN_PULSISH, 3, {
      0x00AB55,
      0x8D1C55,
      0x00001C
    }
  },

  {
    PATTERN_ZIGZAG, 6, {
      RGB_OFF,
      0x56D400,
      0x5500AB,
      RGB_OFF,
      RGB_RED,
      0x700000
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
    PATTERN_SNOWBALL, 3, {
      0x170600,
      0x00840A,
      0x12002A
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
    PATTERN_VORTEX, 4, {
      0xAA0055,
      0x7070C5,
      0x0A0013,
      0x1C8E55,
    }
  },
  {
    PATTERN_VORTEXWIPE, 8, {
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
    PATTERN_VORTEXWIPE, 3, {
      0x00AB55,
      0x7F0081,
      0xAA381C,
    }
  },
  {
    PATTERN_COMPLEMENTARY_BLEND, 3, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE
    }
  },
  {
    PATTERN_STROBE, 2, {
      RGB_RED,
      RGB_GREEN,
    }
  },
  {
    PATTERN_DRIP, 4, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE,
      RGB_YELLOW
    }
  },
  {
    PATTERN_DASHCYCLE, 3, {
      RGB_RED,
      RGB_GREEN,
      RGB_BLUE
    }
  }



#endif // POV_TEST_MODES
};

// exposed size of the default modes array
const uint8_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));

// compile time assert for max modes count check
static_assert(MAX_MODES == (sizeof(default_modes) / sizeof(default_modes[0])),
  "Incorrect number of default modes! Ensure default_modes array has "
  EXPAND_AND_QUOTE(MAX_MODES) " enties");
