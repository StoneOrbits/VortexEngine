#include "DefaultModes.h"

#include "../Colors/ColorTypes.h"

// Here is the array of 'default modes' that are assigned to
// the gloveset upon factory reset
const DefaultModeEntry defaultModes[MAX_MODES] = {
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
  },
  {
    {
      {
       PATTERN_RIBBON, 8, {
         0x6a0000,
         0xf3ff00,
         0x6c00ff,
         0x00fe89,
         0x003fff,
         0xff00bd,
         0x00ff3c,
         0x5057ff,
        }
      },
      {
       PATTERN_HYPERSTROBE, 8, {
         0x6a0000,
         0xf3ff00,
         0x6c00ff,
         0x00fe89,
         0x003fff,
         0xff00bd,
         0x00ff3c,
         0x5057ff,
        }
      }
    }
  },
  {
    {
      {
       PATTERN_STROBE, 4, {
         0x6c00ff,
         0x00fe89,
         0xff00bd,
         0x00ff3c,
        }
      },
      {
       PATTERN_HYPERSTROBE, 2, {
         0x6a0000,
         0xf3ff00,
        }
      }
    }
  },
  {
    {
      {
        PATTERN_BLEND, 2, {
          RGB_RED0,
          RGB_CYAN,
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

};
