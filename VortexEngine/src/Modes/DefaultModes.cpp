#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const DefaultModeEntry defaultModes[] = {
  {
    {
      {
        PATTERN_STROBEGAP, 4, {
          RGB_RED,
          RGB_GREEN,
          RGB_BLUE,
          0x000000
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
        PATTERN_COMPLEMENTARY_BLENDSTROBEGAP, 6, {
          RGB_RED5,
          RGB_YELLOW5,
          RGB_RED5,
          RGB_CYAN5,
          RGB_BLUE5,
          RGB_CYAN5,
        }
      },
      {
        PATTERN_COMPLEMENTARY_BLENDSTROBEGAP, 6, {
          RGB_RED5,
          RGB_YELLOW5,
          RGB_RED5,
          RGB_CYAN5,
          RGB_BLUE5,
          RGB_CYAN5,
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
