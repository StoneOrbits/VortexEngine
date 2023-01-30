#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>
#include <stdarg.h>

#include "../VortexConfig.h"

// Defined the LED positions, their order, and index
enum LedPos : uint8_t
{
  // this should always be first
  LED_FIRST = 0,

  // the first quadrant middle to outside
  LED_0 = LED_FIRST,
  LED_1,
  LED_2,

  // first quadrant tip
  LED_3,

  // the first quadrant outside to middle
  LED_4,
  LED_5,
  LED_6,

  // the second quadrant middle to outside
  LED_7,
  LED_8,
  LED_9,

  // the second quadrant tip
  LED_10,

  // the second quadrant outside to middle
  LED_11,
  LED_12,
  LED_13,

  // the third quadrant middle to outside
  LED_14,
  LED_15,
  LED_16,

  // the third quadrant tip
  LED_17,

  // the third quadrant outside to middle
  LED_18,
  LED_19,
  LED_20,

  // the fourth quadrant middle to outside
  LED_21,
  LED_22,
  LED_23,

  // the fourth quadrant tip
  LED_24,

  // the fourth quadrant outside to middle
  LED_25,
  LED_26,
  LED_27,

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1)
};

enum Quadrant : uint8_t
{
  QUADRANT_FIRST = 0,

  QUADRANT_1 = QUADRANT_FIRST,
  QUADRANT_2,
  QUADRANT_3,
  QUADRANT_4,
  QUADRANT_5, // This may look confusing but the extra "quadrant" improves compatibility. The name could improve

  QUADRANT_COUNT, // 5
  QUADRANT_LAST = (QUADRANT_COUNT - 1)
};

enum Ring : uint8_t
{
  RING_FIRST = 0,
  RING_1 = RING_FIRST,
  RING_2,
  RING_3,
  RING_4,
  RING_COUNT,
  RING_LAST = (RING_COUNT - 1)
};

enum LedPair : uint8_t
{
  PAIR_FIRST = 0,

  // Quadrant 1
  PAIR_1 = PAIR_FIRST,
  PAIR_2,
  PAIR_3,
  PAIR_4,
  // Quadrant 2
  PAIR_5,
  PAIR_6,
  PAIR_7,
  PAIR_8,
  // Quadrant 3
  PAIR_9,
  PAIR_10,
  PAIR_11,
  PAIR_12,
  // Quadrant 4
  PAIR_13,
  PAIR_14,
  PAIR_15,
  PAIR_16,

  PAIR_COUNT,
  PAIR_LAST = (PAIR_COUNT - 1),

};

// check if an led is finger tip or top
inline bool isEven(LedPos pos)
{
  return (pos % 2) == 0;
}
inline bool isOdd(LedPos pos)
{
  return (pos % 2) != 0;
}

// check if led is on the top side
inline bool isPairTop(LedPos pos)
{
  switch (pos) {
  case LED_0: case LED_1: case LED_2:
  case LED_11: case LED_12: case LED_13:
  case LED_14: case LED_15: case LED_16:
  case LED_25: case LED_26: case LED_27:
    return true;
  default:
    return false;
  }
}

// check if led is on the bottom side
inline bool isPairBot(LedPos pos)
{
  switch (pos) {
  case LED_4: case LED_5: case LED_6:
  case LED_7: case LED_8: case LED_9:
  case LED_18: case LED_19: case LED_20:
  case LED_21: case LED_22: case LED_23:
    return true;
  default:
    return false;
  }
}

// check if led is on the side
inline bool isPairSide(LedPos pos)
{
  switch (pos) {
  case LED_3: case LED_10: case LED_17: case LED_24:
    return true;
  default:
    return false;
  }
}

// convert and led postion to a pair
inline LedPair ledToPair(LedPos pos)
{
  switch (pos) {
  // Quadrant 1
  case LED_0:
  case LED_6:
    return PAIR_1;
  case LED_1:
  case LED_5:
    return PAIR_2;
  case LED_2:
  case LED_4:
    return PAIR_3;
  case LED_3:
    return PAIR_4;

    // Quadrant 2
  case LED_7:
  case LED_13:
    return PAIR_5;
  case LED_8:
  case LED_12:
    return PAIR_6;
  case LED_9:
  case LED_11:
    return PAIR_7;
  case LED_10:
    return PAIR_8;

    // Quadrant 3
  case LED_14:
  case LED_20:
    return PAIR_9;
  case LED_15:
  case LED_19:
    return PAIR_10;
  case LED_16:
  case LED_18:
    return PAIR_11;
  case LED_17:
    return PAIR_12;

    // Quadrant 4
  case LED_21:
  case LED_27:
    return PAIR_13;
  case LED_22:
  case LED_26:
    return PAIR_14;
  case LED_23:
  case LED_25:
    return PAIR_15;
  case LED_24:
    return PAIR_16;

  default:
    return PAIR_FIRST;
  }
}

// get the top led from the pair
inline LedPos pairTop(LedPair pair)
{
  switch (pair) {
  case PAIR_1:
  default:
    return LED_0;
  case PAIR_2:
    return LED_1;
  case PAIR_3:
    return LED_2;
  case PAIR_4:
    return LED_3;
  case PAIR_5:
    return LED_13;
  case PAIR_6:
    return LED_12;
  case PAIR_7:
    return LED_11;
  case PAIR_8:
    return LED_10;
  case PAIR_9:
    return LED_14;
  case PAIR_10:
    return LED_15;
  case PAIR_11:
    return LED_16;
  case PAIR_12:
    return LED_17;
  case PAIR_13:
    return LED_27;
  case PAIR_14:
    return LED_26;
  case PAIR_15:
    return LED_25;
  case PAIR_16:
    return LED_24;
  }
}

// get the bottom led from the pair
inline LedPos pairBot(LedPair pair)
{
  switch (pair) {
  case PAIR_1:
  default:
    return LED_6;
  case PAIR_2:
    return LED_5;
  case PAIR_3:
    return LED_4;
  case PAIR_4:
    return LED_3;
  case PAIR_5:
    return LED_7;
  case PAIR_6:
    return LED_8;
  case PAIR_7:
    return LED_9;
  case PAIR_8:
    return LED_10;
  case PAIR_9:
    return LED_20;
  case PAIR_10:
    return LED_19;
  case PAIR_11:
    return LED_18;
  case PAIR_12:
    return LED_17;
  case PAIR_13:
    return LED_21;
  case PAIR_14:
    return LED_22;
  case PAIR_15:
    return LED_23;
  case PAIR_16:
    return LED_24;
  }
}

// get the first led in the quadrant
inline LedPos quadrantFirstLed(Quadrant quadrant)
{
  return (LedPos)((uint32_t)quadrant * 7);
}

// get the middle led in the quadrant
inline LedPos quadrantMiddleLed(Quadrant quadrant)
{
  return (LedPos)(((uint32_t)quadrant * 7) + 3);
}

// get the last led in the quadrant
inline LedPos quadrantLastLed(Quadrant quadrant)
{
  return (LedPos)(((uint32_t)quadrant * 7) + 6);
}

// LedMap is a bitmap of leds, used for expressing whether to turn certain leds on
// or off with a single integer
typedef uint64_t LedMap;

// various macros for mapping leds to an LedMap
#define MAP_LED(led) (1 << led)

// bitmap of all fingers (basically LED_COUNT bits)
#define MAP_LED_ALL ((2 << (LED_COUNT - 1)) - 1)

#define MAP_INVERSE(map) ((~map) & MAP_LED_ALL)

// set a single led
inline void setLed(LedMap map, LedPos pos)
{
  if (pos < LED_COUNT) map |= (1ull << pos);
}

// check if an led is set in the map
inline bool checkLed(LedMap map, LedPos pos)
{
  return ((map & (1ull << pos)) != 0);
}

// LedPos operators
inline LedPos &operator++(LedPos &c)
{
  c = LedPos(((uint32_t)c) + 1);
  return c;
}
inline LedPos operator++(LedPos &c, int)
{
  LedPos temp = c;
  ++c;
  return temp;
}
inline LedPos operator+(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c + b);
}
inline LedPos &operator+=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) + b);
  return c;
}
inline LedPos operator-(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c - b);
}
inline LedPos &operator-=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) - b);
  return c;
}

// Quadrant operators
inline Quadrant &operator++(Quadrant &c)
{
  c = Quadrant(((uint32_t)c) + 1);
  return c;
}
inline Quadrant operator++(Quadrant &c, int)
{
  Quadrant temp = c;
  ++c;
  return temp;
}
inline Quadrant operator+(Quadrant &c, int b)
{
  return (Quadrant)((uint32_t)c + b);
}

inline Quadrant &operator+=(Quadrant &c, int b)
{
  c = Quadrant(((uint32_t)c) + b);
  return c;
}

inline Quadrant operator-(Quadrant &c, int b)
{
  return (Quadrant)((uint32_t)c - b);
}

inline Quadrant &operator-=(Quadrant &c, int b)
{
  c = Quadrant(((uint32_t)c) - b);
  return c;
}

// Ring operators
inline Ring &operator++(Ring &c)
{
  c = Ring(((uint32_t)c) + 1);
  return c;
}
inline Ring operator++(Ring &c, int)
{
  Ring temp = c;
  ++c;
  return temp;
}
inline Ring operator+(Ring &c, int b)
{
  return (Ring)((uint32_t)c + b);
}

inline Ring &operator+=(Ring &c, int b)
{
  c = Ring(((uint32_t)c) + b);
  return c;
}

inline Ring operator-(Ring &c, int b)
{
  return (Ring)((uint32_t)c - b);
}

inline Ring &operator-=(Ring &c, int b)
{
  c = Ring(((uint32_t)c) - b);
  return c;
}

// Pair operators
inline LedPair &operator++(LedPair &c)
{
  c = LedPair(((uint32_t)c) + 1);
  return c;
}
inline LedPair operator++(LedPair &c, int)
{
  LedPair temp = c;
  ++c;
  return temp;
}
inline LedPair operator+(LedPair &c, int b)
{
  return (LedPair)((uint32_t)c + b);
}
inline LedPair operator-(LedPair &c, int b)
{
  return (LedPair)((uint32_t)c - b);
}
#endif
