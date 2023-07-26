#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const DefaultModeEntry default_modes[] = {
  {
    {
      {
        PATTERN_STROBEGAP, 3, {
          RGB_RED,
          RGB_GREEN,
          RGB_BLUE,
        }
      },
      {
        PATTERN_STROBEGAP, 3, {
          RGB_RED,
          RGB_GREEN,
          RGB_BLUE,
        }
      }
    }
  },
  {
    {
      {
        PATTERN_BLEND, 3, {
          RGB_RED,
          RGB_GREEN,
          RGB_BLUE,
        }
      },
      {
        PATTERN_BLEND, 3, {
          RGB_RED,
          RGB_GREEN,
          RGB_BLUE,
        }
      }
    }
  },
  {
    {
      {
        PATTERN_STROBE, 1, {
          0x97709F,
        }
      },
      {
        PATTERN_HYPERGAP, 1, {
          0x4D00B2,
        }
      }
    }
  },
  {
    {
      {
        PATTERN_DOPGAP, 8, {
          RGB_OFF,
          0x56D400,
          0x5500AB,
          RGB_OFF,
          RGB_RED,
          0x700000,
          RGB_PURPLE,
          0x700000
        }
      },
      {
        PATTERN_DOPGAP, 8, {
          RGB_OFF,
          0x56D400,
          0x5500AB,
          RGB_OFF,
          RGB_RED,
          0x700000,
          RGB_PURPLE,
          0x700000
        }
      }
    }
  },
  {
    {
      {
        PATTERN_DOUBLEDOPS, 8, {
          0xFFC600,
          0x235500,
          0x00FF66,
          0x004355,
          0x0600FF,
          0x480055,
          0xFF0057,
          0x552C00,
        }
      },
      {
        PATTERN_DOUBLEDOPS, 8, {
          0xFFC600,
          0x235500,
          0x00FF66,
          0x004355,
          0x0600FF,
          0x480055,
          0xFF0057,
          0x552C00,
        }
      }
    }
  }
};

// exposed size of the default modes array
const uint8_t num_default_modes = (sizeof(default_modes) / sizeof(default_modes[0]));
