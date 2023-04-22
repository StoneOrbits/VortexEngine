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

  // LED constants for each led
  LED_0 = LED_FIRST,
  LED_1,

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1),

  // target all leds (multi and single)
  LED_ALL = LED_COUNT,

  // target the multi led slot
  LED_MULTI = (LED_COUNT + 1),

  // target all single led slots
  LED_ALL_SINGLE = (LED_COUNT + 2),

  // Target the 'effective' led slot (any slot)
  //
  // When fetching this will:
  //    1. return the multi led slot if it exists
  //    2. otherwise the first single led slot
  // 
  // When setting this will:
  //    1. if setting single led pattern will set all
  //    2. if setting multi led pattern will set multi
  //    3. will unset the opposite type of pattern
  LED_ANY = (LED_COUNT + 3),

  // other customs?
  // LED_EVENS = (LED_COUNT + 2),
  // LED_ODDS = (LED_COUNT + 3),
};

// some helpers for microlight code
#define LED_TIP LED_0
#define LED_TOP LED_1

enum Pair : uint8_t
{
  PAIR_FIRST = 0,

  // one pair for each pair of leds, adjust this to be 2x the LED_COUNT
  PAIR_0 = PAIR_FIRST,

  PAIR_COUNT,
  PAIR_LAST = (PAIR_COUNT - 1),
};

// backwards compat with bigger patterns
#define LED_2 LED_0
#define LED_3 LED_1
#define LED_4 LED_0
#define LED_5 LED_1
#define LED_6 LED_0
#define LED_7 LED_1
#define LED_8 LED_0
#define LED_9 LED_1

#define PAIR_1 PAIR_0
#define PAIR_2 PAIR_0
#define PAIR_3 PAIR_0
#define PAIR_4 PAIR_0

// check if an led is even or odd
#define isEven(pos) ((pos % 2) == 0)
#define isOdd(pos) ((pos % 2) != 0)

// convert a pair to even or odd led position
#define pairEven(pair) (LedPos)((uint32_t)pair * 2)
#define pairOdd(pair) (LedPos)(((uint32_t)pair * 2) + 1)

// convert an led position to a pair
#define ledToPair(pos) (Pair)((uint32_t)pos / 2)

// LedMap is a bitmap of leds, used for expressing whether to turn certain leds on
// or off with a single integer
typedef uint64_t LedMap;

// various macros for mapping leds to an LedMap
#define MAP_LED(led) (LedMap)((uint64_t)1 << led)
#define MAP_PAIR_EVEN(pair) MAP_LED(pairEven(pair))
#define MAP_PAIR_ODD(pair) MAP_LED(pairOdd(pair))
#define MAP_PAIR(pair) (MAP_PAIR_EVEN(pair) | MAP_PAIR_ODD(pair))

// check if a map is purely just 1 led or not
#define MAP_IS_ONE_LED(map) (map && !(map & (map-1)))

// foreach led macro
#define MAP_FOREACH_LED(map) for (LedPos pos = mapGetFirstLed(map); pos < LED_COUNT; pos = mapGetNextLed(map, pos))

// convert a map to the first Led position in the map
inline LedPos mapGetFirstLed(LedMap map)
{
  LedPos pos = LED_FIRST;
  while (map && pos < LED_COUNT) {
    if (map & 1) {
      return pos;
    }
    map >>= 1;
    pos = (LedPos)(pos + 1);
  }
  return LED_COUNT;
}

// given an led map and a position, find the next position in the map
inline LedPos mapGetNextLed(LedMap map, LedPos pos)
{
  pos = (LedPos)(pos + 1);
  map >>= pos;
  while (map && pos < LED_COUNT) {
    if (map & 1) {
      return pos;
    }
    map >>= 1;
    pos = (LedPos)(pos + 1);
  }
  return LED_COUNT;
}

// bitmap of all pairs (basically LED_COUNT bits)
#define MAP_LED_ALL ((2 << (LED_COUNT - 1)) - 1)

#define MAP_INVERSE(map) ((~map) & MAP_LED_ALL)

// macro for all evens and odds
#define MAP_PAIR_EVENS (MAP_PAIR_EVEN(PAIR_0) | MAP_PAIR_EVEN(PAIR_1) | MAP_PAIR_EVEN(PAIR_2) | MAP_PAIR_EVEN(PAIR_3) | MAP_PAIR_EVEN(PAIR_4))
#define MAP_PAIR_ODDS (MAP_PAIR_ODD(PAIR_0) | MAP_PAIR_ODD(PAIR_1) | MAP_PAIR_ODD(PAIR_2) | MAP_PAIR_ODD(PAIR_3) | MAP_PAIR_ODD(PAIR_4))

// Some preset bitmaps for pair groupings
#define MAP_PAIR_ODD_EVENS (MAP_PAIR_EVEN(PAIR_0) | MAP_PAIR_EVEN(PAIR_2) | MAP_PAIR_EVEN(PAIR_4))
#define MAP_PAIR_ODD_ODDS (MAP_PAIR_ODD(PAIR_0) | MAP_PAIR_ODD(PAIR_2) | MAP_PAIR_ODD(PAIR_4))

#define MAP_PAIR_EVEN_EVENS (MAP_PAIR_EVEN(PAIR_3) | MAP_PAIR_EVEN(PAIR_1))
#define MAP_PAIR_EVEN_ODDS (MAP_PAIR_ODD(PAIR_3) | MAP_PAIR_ODD(PAIR_1))

// set a single led
inline void setLed(LedMap &map, LedPos pos)
{
  if (pos < LED_COUNT) map |= (1ull << pos);
}
// set a single pair
inline void setPair(LedMap &map, Pair pair)
{
  setLed(map, pairEven(pair));
  setLed(map, pairOdd(pair));
}

// check if an led is set in the map
inline bool checkLed(LedMap map, LedPos pos)
{
  return ((map & (1ull << pos)) != 0);
}
// check if a pair is set in the map (both leds)
inline bool checkPair(LedMap map, Pair pair)
{
  return checkLed(map, pairEven(pair)) && checkLed(map, pairOdd(pair));
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

// pair operators
inline Pair &operator++(Pair &c)
{
  c = Pair(((uint32_t)c) + 1);
  return c;
}
inline Pair operator++(Pair &c, int)
{
  Pair temp = c;
  ++c;
  return temp;
}
inline Pair operator+(Pair &c, int b)
{
  return (Pair)((uint32_t)c + b);
}
inline Pair operator-(Pair &c, int b)
{
  return (Pair)((uint32_t)c - b);
}

#endif
