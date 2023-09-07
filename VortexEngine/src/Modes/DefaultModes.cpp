#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const DefaultModeEntry defaultModes[] = {
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
        PATTERN_COMPLEMENTARY_BLEND, 3, {
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
        PATTERN_STROBE, 6, {
          0xC4003B,
          0x700000,
          0x000000,
          0x0017E9,
          0x22004E,
          0x000000,
        }
      },
      {
        PATTERN_STROBIE, 3, {
          0xC4003B,
          0x4D00B2,
          0x0017E9,
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
