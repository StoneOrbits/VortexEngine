#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>
#include <stdarg.h>

#include "../Leds/Leds.h"
#include "../VortexConfig.h"

// check if an led is even or odd
#define isEven(pos) ((pos % 2) == 0)
#define isOdd(pos) ((pos % 2) != 0)

// convert a pair to even or odd led position
#define pairEven(pair) (LedPos)((uint32_t)pair * 2)
#define pairOdd(pair) (LedPos)(((uint32_t)pair * 2) + 1)

// convert an led position to a pair
#define ledToPair(pos) (Pair)((uint32_t)pos / 2)

// various macros for mapping leds to an LedMap
#define MAP_LED(led) (LedMap)((uint64_t)1 << led)
#define MAP_PAIR_EVEN(pair) MAP_LED(pairEven(pair))
#define MAP_PAIR_ODD(pair) MAP_LED(pairOdd(pair))
#define MAP_PAIR(pair) (MAP_PAIR_EVEN(pair) | MAP_PAIR_ODD(pair))

// check if a map is purely just 1 led or not
#define MAP_IS_ONE_LED(map) (map && !(map & (map-1)))

// foreach led macro (only iterates singles)
#define MAP_FOREACH_LED(map) for (LedPos pos = mapGetFirstLed(map); pos < LED_COUNT; pos = mapGetNextLed(map, pos))

// convert a map to the first Led position in the map
inline LedPos mapGetFirstLed(LedMap map)
{
  if (map == MAP_LED(LED_MULTI)) {
    return LED_MULTI;
  }
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

// blank map
#define MAP_LED_NONE 0

#define MAP_INVERSE(map) ((~map) & MAP_LED_ALL)

// macro for all evens and odds
#define MAP_PAIR_EVENS (((1 << LED_COUNT) - 1) & 0x55555555)
#define MAP_PAIR_ODDS (((1 << LED_COUNT) - 1) & 0xAAAAAAAA)

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
