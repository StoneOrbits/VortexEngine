#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>

#include "../VortexConfig.h"

typedef uint8_t LedPos;

enum {
  LED_FIRST = 0,
  LED_0 = LED_FIRST,
  LED_1,
  LED_COUNT,
  LED_LAST = (LED_COUNT - 1),
  LED_ALL = LED_COUNT,
  LED_MULTI = (LED_COUNT + 1),
  LED_ALL_SINGLE = (LED_COUNT + 2),
  LED_ANY = (LED_COUNT + 3),
};

#define LED_TIP LED_0
#define LED_TOP LED_1

typedef uint8_t Pair;

enum {
  PAIR_FIRST = 0,
  PAIR_0 = PAIR_FIRST,
  PAIR_COUNT,
  PAIR_LAST = (PAIR_COUNT - 1),
};

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

#define isEven(pos) ((pos % 2) == 0)
#define isOdd(pos) ((pos % 2) != 0)

#define pairEven(pair) ((LedPos)((uint32_t)pair * 2))
#define pairOdd(pair) ((LedPos)(((uint32_t)pair * 2) + 1))

#define ledToPair(pos) ((Pair)((uint32_t)pos / 2))

typedef uint64_t LedMap;

#define MAP_LED(led) ((LedMap)((uint64_t)1 << led))
#define MAP_PAIR_EVEN(pair) MAP_LED(pairEven(pair))
#define MAP_PAIR_ODD(pair) MAP_LED(pairOdd(pair))
#define MAP_PAIR(pair) (MAP_PAIR_EVEN(pair) | MAP_PAIR_ODD(pair))

#define MAP_IS_ONE_LED(map) (map && !(map & (map-1)))

#define MAP_FOREACH_LED(map) for (LedPos pos = ledmapGetFirstLed(map); pos != LED_COUNT; pos = ledmapGetNextLed(map, pos))

static inline LedPos ledmapGetFirstLed(LedMap map) {
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

static inline LedPos ledmapGetNextLed(LedMap map, LedPos pos) {
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

#define MAP_LED_ALL ((2 << (LED_COUNT - 1)) - 1)
#define MAP_LED_NONE 0
#define MAP_INVERSE(map) ((~map) & MAP_LED_ALL)
#define MAP_PAIR_EVENS (((1 << LED_COUNT) - 1) & 0x55555555)
#define MAP_PAIR_ODDS (((1 << LED_COUNT) - 1) & 0xAAAAAAAA)
#define MAP_PAIR_ODD_EVENS (MAP_PAIR_EVEN(PAIR_0) | MAP_PAIR_EVEN(PAIR_2) | MAP_PAIR_EVEN(PAIR_4))
#define MAP_PAIR_ODD_ODDS (MAP_PAIR_ODD(PAIR_0) | MAP_PAIR_ODD(PAIR_2) | MAP_PAIR_ODD(PAIR_4))
#define MAP_PAIR_EVEN_EVENS (MAP_PAIR_EVEN(PAIR_3) | MAP_PAIR_EVEN(PAIR_1))
#define MAP_PAIR_EVEN_ODDS (MAP_PAIR_ODD(PAIR_3) | MAP_PAIR_ODD(PAIR_1))

static inline void ledmapSetLed(LedMap *map, LedPos pos) {
  if (pos < LED_COUNT) *map |= (1ull << pos);
}

static inline void ledmapSetPair(LedMap *map, Pair pair) {
  ledmapSetLed(map, pairEven(pair));
  ledmapSetLed(map, pairOdd(pair));
}

static inline bool ledmapCheckLed(LedMap map, LedPos pos) {
  return ((map & (1ull << pos)) != 0);
}

static inline bool ledmapCheckPair(LedMap map, Pair pair) {
  return ledmapCheckLed(map, pairEven(pair)) && ledmapCheckLed(map, pairOdd(pair));
}

static inline LedPos ledPosIncr(LedPos c) { return (LedPos)((uint32_t)c + 1); }
static inline LedPos ledPosAdd(LedPos c, int b) { return (LedPos)((uint32_t)c + (uint32_t)b); }
static inline LedPos ledPosAddEq(LedPos *c, int b) { *c = (LedPos)((uint32_t)(*c) + (uint32_t)b); return *c; }
static inline LedPos ledPosSub(LedPos c, int b) { return (LedPos)((uint32_t)c - (uint32_t)b); }
static inline LedPos ledPosSubEq(LedPos *c, int b) { *c = (LedPos)((uint32_t)(*c) - (uint32_t)b); return *c; }

static inline Pair pairIncr(Pair c) { return (Pair)((uint32_t)c + 1); }
static inline Pair pairAdd(Pair c, int b) { return (Pair)((uint32_t)c + (uint32_t)b); }
static inline Pair pairSub(Pair c, int b) { return (Pair)((uint32_t)c - (uint32_t)b); }

#endif
