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
        PATTERN_GHOSTCRUSH, 5, {
          0x9FFF00,
          0x00FF66,
          0x009FFF,
          0x5A00FF,
          0xFF009F
        }
      },
      {
        PATTERN_GHOSTCRUSH, 5, {
          0x9FFF00,
          0x00FF66,
          0x009FFF,
          0x5A00FF,
          0xFF009F
        }
      }
    }
  },

  {
    {
      {
        PATTERN_ULTRADOPS, 5, {
          0x00AA90,
          0x00B1FF,
          0x300055,
          0xFF002D,
          0x541B00
        }
      },
      {
        PATTERN_ULTRADOPS, 5, {
          0x00AA90,
          0x00B1FF,
          0x300055,
          0xFF002D,
          0x541B00
        }
      }
    }
  },

  {
    {
      {
        PATTERN_PICOGAP, 5, {
          0x0000FF,
          0x400055,
          0x54000B,
          0x525400,
          0x26AA00
        }
      },
      {
        PATTERN_PICOGAP, 8, {
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
        PATTERN_DOPS, 2, {
          0xFFF600,
          0x000880
        }
      },
      {
        PATTERN_HYPERSTROBE, 2, {
          0xFFF600,
          0x000880
        }
      }
    }
  },

};
