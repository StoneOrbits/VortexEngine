#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>
#include <stdarg.h>

#include "../VortexConfig.h"

// Defined the LED positions, their order, and index
// The Vortex is shaped like an X: four diagonal arms (quadrants) with 8 leds
// along each side of the board per arm, plus one large led at the tip of each
// arm. This is the same structure as the original Vortex (Orbit) which had 3
// leds per side per arm.
//
// The LedPos values below are LOGICAL positions laid out exactly like the
// Orbit so that all of the patterns can reason about quadrants/rings/sides:
//
//   per quadrant (17 leds):  8 leds middle -> outside, the tip, 8 leds outside -> middle
//
//   quadrant 1: LED_0  - LED_7  out, LED_8  tip, LED_9  - LED_16 in
//   quadrant 2: LED_17 - LED_24 out, LED_25 tip, LED_26 - LED_33 in
//   quadrant 3: LED_34 - LED_41 out, LED_42 tip, LED_43 - LED_50 in
//   quadrant 4: LED_51 - LED_58 out, LED_59 tip, LED_60 - LED_67 in
//
// The physical data chain is wired differently (all 64 small leds first, then
// the four tips in reverse order), see ledToPhysical() which is only used by
// the led driver right before the colors are sent out.
enum LedPos : uint8_t
{
  // this should always be first
  LED_FIRST = 0,

  // LED constants for each led
  LED_0 = LED_FIRST,
  LED_1,
  LED_2,
  LED_3,
  LED_4,
  LED_5,
  LED_6,
  LED_7,
  LED_8,
  LED_9,
  LED_10,
  LED_11,
  LED_12,
  LED_13,
  LED_14,
  LED_15,
  LED_16,
  LED_17,
  LED_18,
  LED_19,
  LED_20,
  LED_21,
  LED_22,
  LED_23,
  LED_24,
  LED_25,
  LED_26,
  LED_27,
  LED_28,
  LED_29,
  LED_30,
  LED_31,
  LED_32,
  LED_33,
  LED_34,
  LED_35,
  LED_36,
  LED_37,
  LED_38,
  LED_39,
  LED_40,
  LED_41,
  LED_42,
  LED_43,
  LED_44,
  LED_45,
  LED_46,
  LED_47,
  LED_48,
  LED_49,
  LED_50,
  LED_51,
  LED_52,
  LED_53,
  LED_54,
  LED_55,
  LED_56,
  LED_57,
  LED_58,
  LED_59,
  LED_60,
  LED_61,
  LED_62,
  LED_63,
  LED_64,
  LED_65,
  LED_66,
  LED_67,

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1),

  // target all leds (multi and single)
  // When fetching this the same as LED_ANY
  // When setting this will set all of the leds
  LED_ALL = LED_COUNT,

  // target the multi led slot
  //
  // When fetching this will return the multi led slot
  // When setting this will set the multi led slot
  LED_MULTI = (LED_COUNT + 1),

  // target all single led slots
  //
  // When fetching this will return the first single led slot
  // When setting this will set all single led slots
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
  LED_ANY = (LED_COUNT + 3),
};

// the shape of the device
#define NUM_QUADRANTS       4
// leds along one side of one arm
#define LEDS_PER_ARM_SIDE   8
// leds in one quadrant: out along one side, the tip, back along the other side
#define LEDS_PER_QUADRANT   ((LEDS_PER_ARM_SIDE * 2) + 1)
// pairs in one quadrant: one per radial position + the tip
#define PAIRS_PER_QUADRANT  (LEDS_PER_ARM_SIDE + 1)

// The quadrants (arms) of the device, QUADRANT_5 is the set of 4 tips
enum Quadrant : uint8_t
{
  QUADRANT_FIRST = 0,

  QUADRANT_1 = QUADRANT_FIRST,
  QUADRANT_2,
  QUADRANT_3,
  QUADRANT_4,
  QUADRANT_5, // the four tips, kept for compatibility with the Orbit

  QUADRANT_COUNT, // 5
  QUADRANT_LAST = (QUADRANT_COUNT - 1)
};

// The rings of the device, from the middle outwards, the last ring is the tips
enum Ring : uint8_t
{
  RING_FIRST = 0,
  RING_1 = RING_FIRST, // innermost
  RING_2,
  RING_3,
  RING_4,
  RING_5,
  RING_6,
  RING_7,
  RING_8, // outermost small leds
  RING_9, // the large tip leds
  RING_COUNT,
  RING_LAST = (RING_COUNT - 1)
};

// A pair is the top and bottom led at the same position on an arm (they are
// directly on top of each other on opposite sides of the board). The tips are
// their own pair, both the 'top' and 'bottom' of a tip pair is the tip led.
//
//   pair = (quadrant * PAIRS_PER_QUADRANT) + ring
enum Pair : uint8_t
{
  PAIR_FIRST = 0,

  PAIR_0 = PAIR_FIRST,
  PAIR_1,
  PAIR_2,
  PAIR_3,
  PAIR_4,
  PAIR_5,
  PAIR_6,
  PAIR_7,
  PAIR_8,
  PAIR_9,
  PAIR_10,
  PAIR_11,
  PAIR_12,
  PAIR_13,
  PAIR_14,
  PAIR_15,
  PAIR_16,
  PAIR_17,
  PAIR_18,
  PAIR_19,
  PAIR_20,
  PAIR_21,
  PAIR_22,
  PAIR_23,
  PAIR_24,
  PAIR_25,
  PAIR_26,
  PAIR_27,
  PAIR_28,
  PAIR_29,
  PAIR_30,
  PAIR_31,
  PAIR_32,
  PAIR_33,
  PAIR_34,
  PAIR_35,

  PAIR_COUNT,
  PAIR_LAST = (PAIR_COUNT - 1),
};

// Compile-time checks on the shape of the device, like the Orbit the tip
// leds are each in their own pair so there are more pairs than LED_COUNT / 2
static_assert(LED_COUNT == (NUM_QUADRANTS * LEDS_PER_QUADRANT), "Incorrect number of Leds for the quadrants");
static_assert(PAIR_COUNT == (NUM_QUADRANTS * PAIRS_PER_QUADRANT), "Incorrect number of Pairs for the quadrants");
static_assert(LED_COUNT == ((PAIR_COUNT * 2) - NUM_QUADRANTS), "Incorrect number of Pairs for Leds! Adjust the Led enum or Pair enum to match");
static_assert(RING_COUNT == PAIRS_PER_QUADRANT, "Incorrect number of Rings for the quadrants");

// check if an led is even or odd
#define isEven(pos) ((pos % 2) == 0)
#define isOdd(pos) ((pos % 2) != 0)

// convert a pair to even or odd led position
#define pairEven(pair) (LedPos)((uint32_t)pair * 2)
#define pairOdd(pair) (LedPos)(((uint32_t)pair * 2) + 1)

// ======================================================================
//  Geometry helpers
//
// These are all constexpr so they can be used to build LedMaps at
// compile time.

// get the quadrant an led is in (tips belong to their arm)
constexpr Quadrant ledToQuadrant(uint32_t pos)
{
  return (Quadrant)(pos / LEDS_PER_QUADRANT);
}

// the position of an led within its quadrant (0 to LEDS_PER_QUADRANT - 1)
constexpr uint32_t ledQuadrantIndex(uint32_t pos)
{
  return pos % LEDS_PER_QUADRANT;
}

// check if led is the tip of an arm
constexpr bool isPairSide(uint32_t pos)
{
  return pos < LED_COUNT && ledQuadrantIndex(pos) == LEDS_PER_ARM_SIDE;
}

// get the ring an led is in, RING_1 is the middle and RING_LAST is the tips
constexpr Ring ledToRing(uint32_t pos)
{
  return (ledQuadrantIndex(pos) <= LEDS_PER_ARM_SIDE)
    ? (Ring)ledQuadrantIndex(pos)
    : (Ring)((LEDS_PER_QUADRANT - 1) - ledQuadrantIndex(pos));
}

// The outward leds of quadrants 1 and 3 are on the top of the board and the
// inward leds are on the bottom, quadrants 2 and 4 are the other way around
constexpr bool isPairTop(uint32_t pos)
{
  return pos < LED_COUNT && !isPairSide(pos) &&
    ((ledQuadrantIndex(pos) < LEDS_PER_ARM_SIDE) == ((ledToQuadrant(pos) % 2) == 0));
}

// check if led is on the bottom side
constexpr bool isPairBot(uint32_t pos)
{
  return pos < LED_COUNT && !isPairSide(pos) && !isPairTop(pos);
}

// check if an led is in an even ring (inner, 3rd, 5th... and the tips)
#define isOrbitEven(pos) ((ledToRing(pos) % 2) == 0)

// convert an led position to a pair
constexpr Pair ledToPair(uint32_t pos)
{
  return (Pair)((ledToQuadrant(pos) * PAIRS_PER_QUADRANT) + ledToRing(pos));
}

// the led going outward / inward on a pair
constexpr LedPos pairOutward(uint32_t pair)
{
  return (LedPos)(((pair / PAIRS_PER_QUADRANT) * LEDS_PER_QUADRANT) + (pair % PAIRS_PER_QUADRANT));
}
constexpr LedPos pairInward(uint32_t pair)
{
  return (LedPos)(((pair / PAIRS_PER_QUADRANT) * LEDS_PER_QUADRANT) + ((LEDS_PER_QUADRANT - 1) - (pair % PAIRS_PER_QUADRANT)));
}

// get the top led from the pair
constexpr LedPos pairTop(uint32_t pair)
{
  return (((pair / PAIRS_PER_QUADRANT) % 2) == 0) ? pairOutward(pair) : pairInward(pair);
}

// get the bottom led from the pair
constexpr LedPos pairBot(uint32_t pair)
{
  return (((pair / PAIRS_PER_QUADRANT) % 2) == 0) ? pairInward(pair) : pairOutward(pair);
}

// get the first led in the quadrant
constexpr LedPos quadrantFirstLed(uint32_t quadrant)
{
  return (LedPos)(quadrant * LEDS_PER_QUADRANT);
}

// get the middle led in the quadrant (the tip)
constexpr LedPos quadrantMiddleLed(uint32_t quadrant)
{
  return (LedPos)((quadrant * LEDS_PER_QUADRANT) + LEDS_PER_ARM_SIDE);
}

// get the last led in the quadrant
constexpr LedPos quadrantLastLed(uint32_t quadrant)
{
  return (LedPos)((quadrant * LEDS_PER_QUADRANT) + (LEDS_PER_QUADRANT - 1));
}

// Convert a logical led position to the index of that led on the physical
// data chain. The chain on the board is:
//   chain 0  - 15: quadrant 1, out along the top then back along the bottom
//   chain 16 - 31: quadrant 2, out along the bottom then back along the top
//   chain 32 - 47: quadrant 3, out along the top then back along the bottom
//   chain 48 - 63: quadrant 4, out along the bottom then back along the top
//   chain 64 - 67: the tips of quadrant 4, 3, 2, 1 (RGB1 - RGB4)
// So it matches the logical layout except the tips are pulled out to the end
constexpr uint8_t ledToPhysical(uint32_t pos)
{
  return isPairSide(pos)
    ? (uint8_t)((LED_COUNT - 1) - ledToQuadrant(pos))
    : (uint8_t)((ledToQuadrant(pos) * (LEDS_PER_ARM_SIDE * 2)) +
        ((ledQuadrantIndex(pos) < LEDS_PER_ARM_SIDE) ? ledQuadrantIndex(pos) : (ledQuadrantIndex(pos) - 1)));
}

// LedMap is a bitmap of leds, used for expressing whether to turn certain leds on
// or off with a single value.
//
// Previously this was a uint64_t but this device has more than 64 leds and the
// map must also be able to hold the special positions (LED_ALL, LED_MULTI, etc)
// which sit just past LED_COUNT. The ESP32-C3 is a 32bit RISC-V core with no
// 128bit integer type, so the map is a small fixed-size array of 32bit words
// with the bitwise operators implemented on top.
#define LEDMAP_BITS (LED_ANY + 1)
#define LEDMAP_WORDS ((LEDMAP_BITS + 31) / 32)

struct LedMap
{
  uint32_t words[LEDMAP_WORDS];

  constexpr LedMap() : words{} {}

  // construct a map from a raw bitfield of the lowest 64 leds, this is explicit
  // so that an LedPos can't accidentally be used as an LedMap
  constexpr explicit LedMap(uint64_t lowBits) : words{}
  {
    words[0] = (uint32_t)lowBits;
    if (LEDMAP_WORDS > 1) {
      words[LEDMAP_WORDS > 1 ? 1 : 0] |= (uint32_t)(lowBits >> 32);
    }
  }

  // build a map with only the given led set
  static constexpr LedMap fromLed(uint32_t pos)
  {
    LedMap map;
    if (pos < LEDMAP_BITS) {
      map.words[pos / 32] = ((uint32_t)1 << (pos % 32));
    }
    return map;
  }

  // build a map with leds [first, last] set
  static constexpr LedMap fromRange(uint32_t first, uint32_t last)
  {
    LedMap map;
    for (uint32_t pos = first; pos <= last && pos < LEDMAP_BITS; ++pos) {
      map.words[pos / 32] |= ((uint32_t)1 << (pos % 32));
    }
    return map;
  }

  // build a map of every Nth led starting at an offset, within [0, LED_COUNT)
  static constexpr LedMap fromStep(uint32_t step, uint32_t offset)
  {
    LedMap map;
    for (uint32_t pos = offset; step && pos < LED_COUNT; pos += step) {
      map.words[pos / 32] |= ((uint32_t)1 << (pos % 32));
    }
    return map;
  }

  constexpr bool check(uint32_t pos) const
  {
    return pos < LEDMAP_BITS && (words[pos / 32] & ((uint32_t)1 << (pos % 32))) != 0;
  }

  constexpr void set(uint32_t pos)
  {
    if (pos < LEDMAP_BITS) words[pos / 32] |= ((uint32_t)1 << (pos % 32));
  }

  constexpr void clear(uint32_t pos)
  {
    if (pos < LEDMAP_BITS) words[pos / 32] &= ~((uint32_t)1 << (pos % 32));
  }

  constexpr bool isEmpty() const
  {
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) {
      if (words[i]) return false;
    }
    return true;
  }

  constexpr uint32_t count() const
  {
    uint32_t total = 0;
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) {
      for (uint32_t w = words[i]; w; w &= (w - 1)) {
        ++total;
      }
    }
    return total;
  }

  constexpr explicit operator bool() const { return !isEmpty(); }

  constexpr bool operator==(const LedMap &other) const
  {
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) {
      if (words[i] != other.words[i]) return false;
    }
    return true;
  }
  constexpr bool operator!=(const LedMap &other) const { return !(*this == other); }

  constexpr LedMap operator|(const LedMap &other) const
  {
    LedMap out;
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) out.words[i] = words[i] | other.words[i];
    return out;
  }
  constexpr LedMap operator&(const LedMap &other) const
  {
    LedMap out;
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) out.words[i] = words[i] & other.words[i];
    return out;
  }
  constexpr LedMap operator^(const LedMap &other) const
  {
    LedMap out;
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) out.words[i] = words[i] ^ other.words[i];
    return out;
  }
  constexpr LedMap operator~() const
  {
    LedMap out;
    for (uint32_t i = 0; i < LEDMAP_WORDS; ++i) out.words[i] = ~words[i];
    return out;
  }
  constexpr LedMap &operator|=(const LedMap &other) { return *this = (*this | other); }
  constexpr LedMap &operator&=(const LedMap &other) { return *this = (*this & other); }
  constexpr LedMap &operator^=(const LedMap &other) { return *this = (*this ^ other); }
};

// various macros for mapping leds to an LedMap
#define MAP_LED(led) LedMap::fromLed((uint32_t)(led))
#define MAP_PAIR_TOP(pair) MAP_LED(pairTop(pair))
#define MAP_PAIR_BOT(pair) MAP_LED(pairBot(pair))
#define MAP_PAIR(pair) (MAP_PAIR_TOP(pair) | MAP_PAIR_BOT(pair))

// check if a map is purely just 1 led or not
#define MAP_IS_ONE_LED(map) ((map).count() == 1)

// foreach led macro (only iterates singles)
#define MAP_FOREACH_LED(map) for (LedPos pos = ledmapGetFirstLed(map); pos != LED_COUNT; pos = ledmapGetNextLed(map, pos))

// bitmap of all leds (LED_COUNT bits)
#define MAP_LED_ALL LedMap::fromRange(LED_FIRST, LED_LAST)

// blank map
#define MAP_LED_NONE LedMap()

#define MAP_INVERSE(map) ((~(map)) & MAP_LED_ALL)

// macro for all evens and odds
#define MAP_PAIR_EVENS LedMap::fromStep(2, 0)
#define MAP_PAIR_ODDS LedMap::fromStep(2, 1)

// build a map of all leds that match a geometry predicate
constexpr LedMap ledmapRing(uint32_t ring)
{
  LedMap map;
  for (uint32_t pos = 0; pos < LED_COUNT; ++pos) {
    if (ledToRing(pos) == ring) map.set(pos);
  }
  return map;
}
constexpr LedMap ledmapRingParity(uint32_t parity)
{
  LedMap map;
  for (uint32_t pos = 0; pos < LED_COUNT; ++pos) {
    if ((ledToRing(pos) % 2) == parity) map.set(pos);
  }
  return map;
}
constexpr LedMap ledmapSide(bool top)
{
  LedMap map;
  for (uint32_t pos = 0; pos < LED_COUNT; ++pos) {
    if (top ? isPairTop(pos) : isPairBot(pos)) map.set(pos);
  }
  return map;
}
constexpr LedMap ledmapQuadrant(uint32_t quadrant)
{
  return LedMap::fromRange(quadrantFirstLed(quadrant), quadrantLastLed(quadrant));
}

// all top leds
#define MAP_ALL_TOP ledmapSide(true)
// all bottom leds
#define MAP_ALL_BOT ledmapSide(false)
// all face leds (everything except the tips)
#define MAP_ALL_FACE (MAP_ALL_TOP | MAP_ALL_BOT)

// led ring maps
#define MAP_RING(ring)  ledmapRing(ring)
#define MAP_RING_INNER  ledmapRing(RING_1)
#define MAP_RING_MIDDLE (ledmapRing(RING_4) | ledmapRing(RING_5))
#define MAP_RING_OUTER  ledmapRing(RING_8)
#define MAP_RING_EDGE   ledmapRing(RING_9)
#define MAP_RINGS_EVEN  ledmapRingParity(0)
#define MAP_RINGS_ODD   ledmapRingParity(1)

// led quadrant maps
#define MAP_QUADRANT_1  ledmapQuadrant(QUADRANT_1)
#define MAP_QUADRANT_2  ledmapQuadrant(QUADRANT_2)
#define MAP_QUADRANT_3  ledmapQuadrant(QUADRANT_3)
#define MAP_QUADRANT_4  ledmapQuadrant(QUADRANT_4)
// the two diagonal lines of the X
#define MAP_LINE_1      (MAP_QUADRANT_1 | MAP_QUADRANT_3)
#define MAP_LINE_2      (MAP_QUADRANT_2 | MAP_QUADRANT_4)

// convert a map to the first Led position in the map
inline LedPos ledmapGetFirstLed(const LedMap &map)
{
  if (map == MAP_LED(LED_MULTI)) {
    return LED_MULTI;
  }
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; pos = (LedPos)(pos + 1)) {
    if (map.check(pos)) {
      return pos;
    }
  }
  return LED_COUNT;
}

// given an led map and a position, find the next position in the map
inline LedPos ledmapGetNextLed(const LedMap &map, LedPos pos)
{
  for (pos = (LedPos)(pos + 1); pos < LED_COUNT; pos = (LedPos)(pos + 1)) {
    if (map.check(pos)) {
      return pos;
    }
  }
  return LED_COUNT;
}

// the number of 32bit words used to serialize a sparse led map for a mode
// with the given number of leds (1 word for 32 or fewer leds)
inline uint32_t ledmapSerialWords(uint32_t numLeds)
{
  return numLeds ? ((numLeds + 31) / 32) : 1;
}

// set a single led
inline void ledmapSetLed(LedMap &map, LedPos pos)
{
  if (pos < LED_COUNT) map.set(pos);
}
// set a single pair
inline void ledmapSetPair(LedMap &map, Pair pair)
{
  ledmapSetLed(map, pairTop(pair));
  ledmapSetLed(map, pairBot(pair));
}

// check if an led is set in the map
inline bool ledmapCheckLed(const LedMap &map, LedPos pos)
{
  return map.check(pos);
}
// check if a pair is set in the map (both leds)
inline bool ledmapCheckPair(const LedMap &map, Pair pair)
{
  return ledmapCheckLed(map, pairTop(pair)) && ledmapCheckLed(map, pairBot(pair));
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
