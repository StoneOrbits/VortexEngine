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

  QUADRANT_COUNT, // 5
  QUADRANT_LAST = (QUADRANT_COUNT - 1),
};

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
  if (pos < LED_COUNT) map |= (1 << pos);
}

// check if an led is set in the map
inline bool checkLed(LedMap map, LedPos pos)
{
  return ((map & (1 << pos)) != 0);
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
inline LedPos& operator+=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) + b);
  return c;
}
inline LedPos operator-(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c - b);
}
inline LedPos& operator-=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) - b);
  return c;
}

#endif
