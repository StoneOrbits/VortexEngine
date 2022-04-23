#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>

// Defined the LED positions, their order, and index
enum LedPos : uint32_t 
{
  // this should always be first
  LED_FIRST = 0,

  // the first should be equal to LED_FIRST
  THUMB_TOP = LED_FIRST,
  THUMB_TIP,

  INDEX_TOP,
  INDEX_TIP,

  MIDDLE_TOP,
  MIDDLE_TIP,

  RING_TOP,
  RING_TIP,

  PINKIE_TOP,
  PINKIE_TIP,

  // INSERT NEW ENTRIES HERE

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1)
};

inline LedPos& operator++(LedPos &c) 
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

inline LedPos operator-(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c - b);
}

#endif
