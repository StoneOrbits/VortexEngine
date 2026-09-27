#ifndef C_TYPES_H
#define C_TYPES_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include "VortexConfig.h"

// =============================================================================
// Forward declarations
// =============================================================================
typedef struct ByteStream ByteStream;
typedef struct BitStream BitStream;
typedef struct Colorset Colorset;
typedef struct Mode Mode;
typedef struct Pattern Pattern;
typedef struct PatternArgs PatternArgs;
typedef struct Menu Menu;
typedef struct Random_t Random_t;
typedef struct Random_t Random;
typedef struct LedStash LedStash;

// =============================================================================
// LedTypes.h
// =============================================================================
typedef uint8_t LedPos;
typedef uint8_t Pair;
typedef uint64_t LedMap;

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

#define pairEven(pair) ((LedPos)((uint32_t)(pair) * 2))
#define pairOdd(pair) ((LedPos)(((uint32_t)(pair) * 2) + 1))
#define ledToPair(pos) ((Pair)((uint32_t)(pos) / 2))

#define MAP_LED(led) ((LedMap)((uint64_t)1 << (led)))
#define MAP_PAIR_EVEN(pair) MAP_LED(pairEven(pair))
#define MAP_PAIR_ODD(pair) MAP_LED(pairOdd(pair))
#define MAP_PAIR(pair) (MAP_PAIR_EVEN(pair) | MAP_PAIR_ODD(pair))
#define MAP_IS_ONE_LED(map) ((map) && !((map) & ((map)-1)))
#define MAP_LED_ALL ((2 << (LED_COUNT - 1)) - 1)
#define MAP_LED_NONE 0
#define MAP_INVERSE(map) ((~(map)) & MAP_LED_ALL)
#define MAP_PAIR_EVENS (((1 << LED_COUNT) - 1) & 0x55555555)
#define MAP_PAIR_ODDS (((1 << LED_COUNT) - 1) & 0xAAAAAAAA)
#define MAP_PAIR_ODD_EVENS (MAP_PAIR_EVEN(PAIR_0) | MAP_PAIR_EVEN(PAIR_2) | MAP_PAIR_EVEN(PAIR_4))
#define MAP_PAIR_ODD_ODDS (MAP_PAIR_ODD(PAIR_0) | MAP_PAIR_ODD(PAIR_2) | MAP_PAIR_ODD(PAIR_4))
#define MAP_PAIR_EVEN_EVENS (MAP_PAIR_EVEN(PAIR_3) | MAP_PAIR_EVEN(PAIR_1))
#define MAP_PAIR_EVEN_ODDS (MAP_PAIR_ODD(PAIR_3) | MAP_PAIR_ODD(PAIR_1))

#define MAP_FOREACH_LED(map) for (LedPos pos = ledmapGetFirstLed(map); pos != LED_COUNT; pos = ledmapGetNextLed(map, pos))

static inline LedPos ledmapGetFirstLed(LedMap map)
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

static inline LedPos ledmapGetNextLed(LedMap map, LedPos pos)
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

static inline bool ledmapCheckLed(LedMap map, LedPos pos)
{
  return ((map & ((LedMap)1 << pos)) != 0);
}

static inline bool ledmapCheckPair(LedMap map, Pair pair)
{
  return ledmapCheckLed(map, pairEven(pair)) && ledmapCheckLed(map, pairOdd(pair));
}

static inline void ledmapSetLed(LedMap *map, LedPos pos)
{
  if (pos < LED_COUNT) {
    *map |= ((LedMap)1 << pos);
  }
}

static inline void ledmapSetPair(LedMap *map, Pair pair)
{
  ledmapSetLed(map, pairEven(pair));
  ledmapSetLed(map, pairOdd(pair));
}

// =============================================================================
// ColorTypes.h + ColorConstants.h
// =============================================================================

typedef enum hsv_to_rgb_algorithm hsv_to_rgb_algorithm;
enum hsv_to_rgb_algorithm {
  HSV_TO_RGB_GENERIC,
  HSV_TO_RGB_RAW,
  HSV_TO_RGB_RAINBOW
};

extern enum hsv_to_rgb_algorithm g_hsv_rgb_alg;

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} RGBColor;

typedef struct {
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
} HSVColor;

void HSVColor_init(HSVColor *self);
void HSVColor_initHSV(HSVColor *self, uint8_t h, uint8_t s, uint8_t v);
void HSVColor_initFromU32(HSVColor *self, uint32_t dwVal);
void HSVColor_assignU32(HSVColor *self, const uint32_t *rhs);
void HSVColor_assignRGB(HSVColor *self, const RGBColor *rhs);
void HSVColor_initFromRGB(HSVColor *self, const RGBColor *rhs);
void HSVColor_assignHSV(HSVColor *self, const HSVColor *rhs);
void HSVColor_clear(HSVColor *self);
bool HSVColor_empty(const HSVColor *self);
uint32_t HSVColor_raw(const HSVColor *self);
bool HSVColor_equals(const HSVColor *self, const HSVColor *other);
void HSVColor_copy(HSVColor *self, const HSVColor *other);

void RGBColor_init(RGBColor *self);
void RGBColor_initRGB(RGBColor *self, uint8_t r, uint8_t g, uint8_t b);
void RGBColor_initFromU32(RGBColor *self, uint32_t dwVal);
void RGBColor_assignU32(RGBColor *self, const uint32_t *rhs);
void RGBColor_assignHSV(RGBColor *self, const HSVColor *rhs);
void RGBColor_assignRGB(RGBColor *self, const RGBColor *rhs);
void RGBColor_clear(RGBColor *self);
bool RGBColor_empty(const RGBColor *self);
uint32_t RGBColor_raw(const RGBColor *self);
bool RGBColor_equals(const RGBColor *self, const RGBColor *other);
void RGBColor_copy(RGBColor *self, const RGBColor *other);
void RGBColor_adjustBrightness(RGBColor *self, uint8_t fadeBy);
bool RGBColor_serialize(const RGBColor *self, ByteStream *buffer);
bool RGBColor_unserialize(RGBColor *self, ByteStream *buffer);
RGBColor RGBColor_adjustBrightnessCopy(const RGBColor *self, uint8_t fadeBy);

RGBColor hsv_to_rgb_rainbow(const HSVColor *rhs);
RGBColor hsv_to_rgb_raw_C(const HSVColor *rhs);
RGBColor hsv_to_rgb_generic(const HSVColor *rhs);
HSVColor rgb_to_hsv_approx(const RGBColor *rhs);
HSVColor rgb_to_hsv_generic(const RGBColor *rhs);

#define HSV_HUE_RED     0
#define HSV_HUE_ORANGE  32
#define HSV_HUE_YELLOW  64
#define HSV_HUE_GREEN   96
#define HSV_HUE_AQUA    128
#define HSV_HUE_BLUE    160
#define HSV_HUE_PURPLE  192
#define HSV_HUE_PINK    224

#define HSV_BIT ((uint32_t)1 << 31)
#ifndef HSV
#define HSV(h, s, v) (HSV_BIT | ((uint32_t)(h) << 16) | ((uint32_t)(s) << 8) | (uint32_t)(v))
#endif

#define HSV_WHITE   HSV_BIT | (uint32_t)0x00006E
#define HSV_BLUE    HSV_BIT | (uint32_t)0xA0FF6E
#define HSV_YELLOW  HSV_BIT | (uint32_t)0x3CFF6E
#define HSV_RED     HSV_BIT | (uint32_t)0x00FF6E
#define HSV_GREEN   HSV_BIT | (uint32_t)0x55FF6E
#define HSV_CYAN    HSV_BIT | (uint32_t)0x78FF6E
#define HSV_PURPLE  HSV_BIT | (uint32_t)0xD4FF6E
#define HSV_ORANGE  HSV_BIT | (uint32_t)0x14FF6E
#define HSV_OFF     HSV_BIT | (uint32_t)0x000000

#define RGB_WHITE       (uint32_t)0xFFFFFF
#define RGB_BLUE        (uint32_t)0x0000FF
#define RGB_YELLOW      (uint32_t)0xFFFF00
#define RGB_RED         (uint32_t)0xFF0000
#define RGB_GREEN       (uint32_t)0x00FF00
#define RGB_CYAN        (uint32_t)0x00FFFF
#define RGB_PURPLE      (uint32_t)0x9933FF
#define RGB_ORANGE      (uint32_t)0xFF8300
#define RGB_PINK        (uint32_t)0xFF0099
#define RGB_MAGENTA     (uint32_t)0xFF00FF
#define RGB_OFF         (uint32_t)0x000000

#define RGB_WHITE0      (uint32_t)0x101010
#define RGB_WHITE1      (uint32_t)0x1C1C1C
#define RGB_WHITE2      (uint32_t)0x383838
#define RGB_WHITE3      (uint32_t)0x545454
#define RGB_WHITE4      (uint32_t)0x707070
#define RGB_WHITE5      (uint32_t)0x8C8C8C
#define RGB_WHITE6      (uint32_t)0xA8A8A8
#define RGB_WHITE7      (uint32_t)0xC4C4C4
#define RGB_WHITE8      (uint32_t)0xE0E0E0
#define RGB_WHITE9      (uint32_t)0xFCFCFC

#define RGB_BLUE0       (uint32_t)0x000010
#define RGB_BLUE1       (uint32_t)0x00001C
#define RGB_BLUE2       (uint32_t)0x000038
#define RGB_BLUE3       (uint32_t)0x000054
#define RGB_BLUE4       (uint32_t)0x000070
#define RGB_BLUE5       (uint32_t)0x00008C
#define RGB_BLUE6       (uint32_t)0x0000A8
#define RGB_BLUE7       (uint32_t)0x0000C4
#define RGB_BLUE8       (uint32_t)0x0000E0
#define RGB_BLUE9       (uint32_t)0x0000FC

#define RGB_YELLOW0     (uint32_t)0x101000
#define RGB_YELLOW1     (uint32_t)0x1C1C00
#define RGB_YELLOW2     (uint32_t)0x383800
#define RGB_YELLOW3     (uint32_t)0x545400
#define RGB_YELLOW4     (uint32_t)0x707000
#define RGB_YELLOW5     (uint32_t)0x8C8C00
#define RGB_YELLOW6     (uint32_t)0xA8A800
#define RGB_YELLOW7     (uint32_t)0xC4C400
#define RGB_YELLOW8     (uint32_t)0xE0E000
#define RGB_YELLOW9     (uint32_t)0xFCFC00

#define RGB_RED0        (uint32_t)0x100000
#define RGB_RED1        (uint32_t)0x1C0000
#define RGB_RED2        (uint32_t)0x380000
#define RGB_RED3        (uint32_t)0x540000
#define RGB_RED4        (uint32_t)0x700000
#define RGB_RED5        (uint32_t)0x8C0000
#define RGB_RED6        (uint32_t)0xA80000
#define RGB_RED7        (uint32_t)0xC40000
#define RGB_RED8        (uint32_t)0xE00000
#define RGB_RED9        (uint32_t)0xFC0000

#define RGB_GREEN0      (uint32_t)0x001000
#define RGB_GREEN1      (uint32_t)0x001C00
#define RGB_GREEN2      (uint32_t)0x003800
#define RGB_GREEN3      (uint32_t)0x005400
#define RGB_GREEN4      (uint32_t)0x007000
#define RGB_GREEN5      (uint32_t)0x008C00
#define RGB_GREEN6      (uint32_t)0x00A800
#define RGB_GREEN7      (uint32_t)0x00C400
#define RGB_GREEN8      (uint32_t)0x00E000
#define RGB_GREEN9      (uint32_t)0x00FC00

#define RGB_CYAN0       (uint32_t)0x001010
#define RGB_CYAN1       (uint32_t)0x001C1C
#define RGB_CYAN2       (uint32_t)0x003838
#define RGB_CYAN3       (uint32_t)0x005454
#define RGB_CYAN4       (uint32_t)0x007070
#define RGB_CYAN5       (uint32_t)0x008C8C
#define RGB_CYAN6       (uint32_t)0x00A8A8
#define RGB_CYAN7       (uint32_t)0x00C4C4
#define RGB_CYAN8       (uint32_t)0x00E0E0
#define RGB_CYAN9       (uint32_t)0x00FCFC

#define RGB_MAGENTA0    (uint32_t)0x100010
#define RGB_MAGENTA1    (uint32_t)0x1C001C
#define RGB_MAGENTA2    (uint32_t)0x380038
#define RGB_MAGENTA3    (uint32_t)0x540054
#define RGB_MAGENTA4    (uint32_t)0x700070
#define RGB_MAGENTA5    (uint32_t)0x8C008C
#define RGB_MAGENTA6    (uint32_t)0xA800A8
#define RGB_MAGENTA7    (uint32_t)0xC400C4
#define RGB_MAGENTA8    (uint32_t)0xE000E0
#define RGB_MAGENTA9    (uint32_t)0xFC00FC

#define RGB_ORANGE0     (uint32_t)0x100800
#define RGB_ORANGE1     (uint32_t)0x1C0E00
#define RGB_ORANGE2     (uint32_t)0x381C00
#define RGB_ORANGE3     (uint32_t)0x542B00
#define RGB_ORANGE4     (uint32_t)0x703900
#define RGB_ORANGE5     (uint32_t)0x8C4800
#define RGB_ORANGE6     (uint32_t)0xA85600
#define RGB_ORANGE7     (uint32_t)0xC46500
#define RGB_ORANGE8     (uint32_t)0xE07300
#define RGB_ORANGE9     (uint32_t)0xFC8200

// =============================================================================
// Colorset.h
// =============================================================================

typedef enum ValueStyle ValueStyle;
enum ValueStyle {
  VAL_STYLE_RANDOM = 0,
  VAL_STYLE_LOW_FIRST_COLOR,
  VAL_STYLE_HIGH_FIRST_COLOR,
  VAL_STYLE_ALTERNATING,
  VAL_STYLE_ASCENDING,
  VAL_STYLE_DESCENDING,
  VAL_STYLE_CONSTANT,
  VAL_STYLE_COUNT
};

typedef enum ColorMode ColorMode;
enum ColorMode {
  COLOR_MODE_THEORY,
  COLOR_MODE_MONOCHROMATIC,
  COLOR_MODE_EVENLY_SPACED
};

typedef enum ColorMode2 ColorMode2;
enum ColorMode2 {
  COLOR_MODE2_DOUBLE_SPLIT_COMPLIMENTARY,
  COLOR_MODE2_TETRADIC
};

struct Colorset {
  RGBColor palette[MAX_COLOR_SLOTS];
  uint8_t curIndex;
  uint8_t numColors;
};

void Colorset_init(Colorset *self);
void Colorset_initColors(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
void Colorset_initFromU32s(Colorset *self, uint8_t numCols, const uint32_t *cols);
void Colorset_destroy(Colorset *self);
void Colorset_copy(Colorset *self, const Colorset *other);
void Colorset_assign(Colorset *self, const Colorset *other);
void Colorset_clear(Colorset *self);
bool Colorset_equals(const Colorset *self, const Colorset *other);
bool Colorset_equalsPtr(const Colorset *self, const Colorset *set);
bool Colorset_addColor(Colorset *self, RGBColor col);
bool Colorset_addColorHSV(Colorset *self, uint8_t hue, uint8_t sat, uint8_t val);
void Colorset_addColorWithValueStyle(Colorset *self, Random *ctx, uint8_t hue, uint8_t sat, ValueStyle valStyle, uint8_t numColors, uint8_t colorPos);
void Colorset_removeColor(Colorset *self, uint8_t index);
void Colorset_randomize(Colorset *self, Random_t *ctx, uint8_t numColors);
void Colorset_randomizeColors(Colorset *self, Random *ctx, uint8_t numColors, ColorMode mode);
void Colorset_randomizeColors2(Colorset *self, Random *ctx, ColorMode2 mode);
void Colorset_adjustBrightness(Colorset *self, uint8_t fadeby);
RGBColor Colorset_get(const Colorset *self, uint8_t index);
void Colorset_set(Colorset *self, uint8_t index, RGBColor col);
void Colorset_skip(Colorset *self, int32_t amount);
RGBColor Colorset_cur(Colorset *self);
void Colorset_setCurIndex(Colorset *self, uint8_t index);
void Colorset_resetIndex(Colorset *self);
uint8_t Colorset_curIndex(const Colorset *self);
RGBColor Colorset_getPrev(Colorset *self);
RGBColor Colorset_getNext(Colorset *self);
RGBColor Colorset_peek(const Colorset *self, int32_t offset);
uint8_t Colorset_numColors(const Colorset *self);
bool Colorset_onStart(const Colorset *self);
bool Colorset_onEnd(const Colorset *self);
bool Colorset_serialize(const Colorset *self, ByteStream *buffer);
bool Colorset_unserialize(Colorset *self, ByteStream *buffer);

// =============================================================================
// Random.h
// =============================================================================

typedef struct Random_t Random;

struct Random_t {
  uint32_t seed;
};

void Random_init(Random_t *self);
void Random_initWithSeed(Random_t *self, uint32_t seed);
void Random_seed(Random_t *self, uint32_t newseed);
uint8_t Random_next8(Random_t *self, uint8_t min, uint8_t max);
uint16_t Random_next16(Random_t *self, uint16_t min, uint16_t max);

// =============================================================================
// Patterns.h
// =============================================================================

typedef int8_t PatternID;

enum {
  PATTERN_NONE = (PatternID)-1,
  PATTERN_FIRST = 0,
  PATTERN_SINGLE_FIRST = PATTERN_FIRST,
  PATTERN_STROBE = PATTERN_FIRST,
  PATTERN_HYPERSTROBE,
  PATTERN_PICOSTROBE,
  PATTERN_STROBIE,
  PATTERN_DOPS,
  PATTERN_ULTRADOPS,
  PATTERN_STROBEGAP,
  PATTERN_HYPERGAP,
  PATTERN_PICOGAP,
  PATTERN_STROBIEGAP,
  PATTERN_DOPSGAP,
  PATTERN_ULTRAGAP,
  PATTERN_BLINKIE,
  PATTERN_GHOSTCRUSH,
  PATTERN_DOUBLEDOPS,
  PATTERN_CHOPPER,
  PATTERN_DASHGAP,
  PATTERN_DASHDOPS,
  PATTERN_DASHCRUSH,
  PATTERN_ULTRADASH,
  PATTERN_GAPCYCLE,
  PATTERN_DASHCYCLE,
  PATTERN_TRACER,
  PATTERN_RIBBON,
  PATTERN_MINIRIBBON,
  PATTERN_BLEND,
  PATTERN_BLENDSTROBE,
  PATTERN_BLENDSTROBEGAP,
  PATTERN_COMPLEMENTARY_BLEND,
  PATTERN_COMPLEMENTARY_BLENDSTROBE,
  PATTERN_COMPLEMENTARY_BLENDSTROBEGAP,
  PATTERN_SOLID,
  PATTERN_MULTI_FIRST,
  PATTERN_SINGLE_LAST = (PATTERN_MULTI_FIRST - 1),
  PATTERN_SINGLE_COUNT = (PATTERN_SINGLE_LAST - PATTERN_SINGLE_FIRST) + 1,
  PATTERN_HUE_SCROLL = PATTERN_MULTI_FIRST,
  PATTERN_THEATER_CHASE,
  PATTERN_CHASER,
  PATTERN_ZIGZAG,
  PATTERN_ZIPFADE,
  PATTERN_DRIP,
  PATTERN_DRIPMORPH,
  PATTERN_CROSSDOPS,
  PATTERN_DOUBLESTROBE,
  PATTERN_METEOR,
  PATTERN_SPARKLETRACE,
  PATTERN_VORTEXWIPE,
  PATTERN_WARP,
  PATTERN_WARPWORM,
  PATTERN_SNOWBALL,
  PATTERN_LIGHTHOUSE,
  PATTERN_PULSISH,
  PATTERN_FILL,
  PATTERN_BOUNCE,
  PATTERN_SPLITSTROBIE,
  PATTERN_BACKSTROBE,
  PATTERN_VORTEX,
  INTERNAL_PATTERNS_END,
  PATTERN_MULTI_LAST = (INTERNAL_PATTERNS_END - 1),
  PATTERN_MULTI_COUNT = (PATTERN_MULTI_LAST - PATTERN_MULTI_FIRST) + 1,
  PATTERN_LAST = PATTERN_MULTI_LAST,
  PATTERN_COUNT = (PATTERN_LAST - PATTERN_FIRST) + 1,
};

static inline bool isMultiLedPatternID(PatternID id)
{
  return id >= PATTERN_MULTI_FIRST && id <= PATTERN_MULTI_LAST;
}

static inline bool isSingleLedPatternID(PatternID id)
{
  return id < PATTERN_MULTI_FIRST;
}

// =============================================================================
// PatternArgs.h
// =============================================================================

#define MAX_ARGS 8

typedef uint8_t ArgMap;

#define ARG_NONE  0
#define ARG(x)    (1 << (x))
#define ARG1      (1 << 0)
#define ARG2      (1 << 1)
#define ARG3      (1 << 2)
#define ARG4      (1 << 3)
#define ARG5      (1 << 4)
#define ARG6      (1 << 5)
#define ARG7      (1 << 6)
#define ARG8      (1 << 7)
#define ARG_ALL   0xFF

#define ARGMAP_SET(map, arg)    ((map) |= ARG(arg))
#define ARGMAP_CLEAR(map, arg)  ((map) &= ~ARG(arg))
#define ARGMAP_ISSET(map, arg)  (((map) & ARG(arg)) != 0)

struct PatternArgs {
  union {
    uint8_t args[8];
    struct {
      uint8_t arg1;
      uint8_t arg2;
      uint8_t arg3;
      uint8_t arg4;
      uint8_t arg5;
      uint8_t arg6;
      uint8_t arg7;
      uint8_t arg8;
    };
    struct {
      uint8_t onDuration;
      uint8_t offDuration;
      uint8_t gapDuration;
      uint8_t dashDuration;
      uint8_t groupSize;
    } basic;
    struct {
      uint8_t onDuration;
      uint8_t offDuration;
      uint8_t gapDuration;
      uint8_t dashDuration;
      uint8_t groupSize;
      uint8_t blendSpeed;
      uint8_t numFlips;
    } blend;
    struct {
      uint8_t onDuration;
      uint8_t offDuration;
      uint8_t gapDuration;
      uint8_t dashDuration;
      uint8_t groupSize;
      uint8_t colorIndex;
    } solid;
  };
  uint8_t numArgs;
};

void PatternArgs_init(PatternArgs *self);
void PatternArgs_init1(PatternArgs *self, uint8_t a1);
void PatternArgs_init2(PatternArgs *self, uint8_t a1, uint8_t a2);
void PatternArgs_init3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3);
void PatternArgs_init4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
void PatternArgs_init5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
void PatternArgs_init6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
void PatternArgs_init7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);
void PatternArgs_init8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);
void PatternArgs_addArgs1(PatternArgs *self, uint8_t a1);
void PatternArgs_addArgs2(PatternArgs *self, uint8_t a1, uint8_t a2);
void PatternArgs_addArgs3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3);
void PatternArgs_addArgs4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
void PatternArgs_addArgs5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
void PatternArgs_addArgs6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
void PatternArgs_addArgs7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);
void PatternArgs_addArgs8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);
bool PatternArgs_equals(const PatternArgs *self, const PatternArgs *rhs);
bool PatternArgs_serialize(const PatternArgs *self, ByteStream *buffer, uint8_t argmap);
uint8_t PatternArgs_unserialize(PatternArgs *self, ByteStream *buffer);

// =============================================================================
// Pattern.h
// =============================================================================

#define MAX_PATTERN_ARGS 8
#define PATTERN_FLAGS_NONE  0
#define PATTERN_FLAG_MULTI  (1<<0)

#if VORTEX_SLIM == 1
typedef uint8_t arg_offset_t;
#else
typedef uint16_t arg_offset_t;
#endif

typedef struct PatternVTable PatternVTable;
struct PatternVTable {
  void (*destroy)(Pattern *self);
  void (*play)(Pattern *self);
  void (*init)(Pattern *self);
  void (*bind)(Pattern *self, LedPos pos);
  void (*onBlinkOn)(Pattern *self);
  void (*onBlinkOff)(Pattern *self);
  void (*beginGap)(Pattern *self);
  void (*beginDash)(Pattern *self);
};

struct Pattern {
  const PatternVTable *vtable;
  PatternID patternID;
  uint8_t patternFlags;
  Colorset colorset;
  LedPos ledPos;
  uint8_t numArgs;
  arg_offset_t argList[MAX_PATTERN_ARGS];
};

void Pattern_init(Pattern *self, const PatternArgs *args);
void Pattern_play(Pattern *self);
void Pattern_bind(Pattern *self, LedPos pos);
void Pattern_bindBase(Pattern *self, LedPos pos);
void Pattern_initBase(Pattern *self);
void Pattern_initVirtual(Pattern *self);
void Pattern_destroy(Pattern *self);
PatternID Pattern_getPatternID(const Pattern *self);
Colorset Pattern_getColorset(const Pattern *self);
void Pattern_setColorset(Pattern *self, const Colorset *set);
void Pattern_clearColorset(Pattern *self);
uint8_t Pattern_getNumArgs(const Pattern *self);
void Pattern_setArg(Pattern *self, uint8_t index, uint8_t value);
uint8_t Pattern_getArg(const Pattern *self, uint8_t index);
void Pattern_setArgs(Pattern *self, const PatternArgs *args);
void Pattern_getArgs(const Pattern *self, PatternArgs *args);
LedPos Pattern_getLedPos(const Pattern *self);
uint32_t Pattern_getFlags(const Pattern *self);
bool Pattern_hasFlags(const Pattern *self, uint32_t flags);
void Pattern_setLedPos(Pattern *self, LedPos pos);
bool Pattern_serialize(const Pattern *self, ByteStream *buffer);
bool Pattern_unserialize(Pattern *self, ByteStream *buffer);
void Pattern_registerArg(Pattern *self, arg_offset_t offset);
bool Pattern_equals(const Pattern *self, const Pattern *other);
#ifdef VORTEX_LIB
void Pattern_skip(Pattern *self, uint32_t ticks);
#endif

#define REGISTER_ARG(self, arg) Pattern_registerArg((Pattern *)(self), (arg_offset_t)((uintptr_t)&(arg) - (uintptr_t)(self)))

// =============================================================================
// SingleLedPattern.h
// =============================================================================
typedef struct {
  Pattern base;
} SingleLedPattern;

void SingleLedPattern_init(SingleLedPattern *self, const PatternArgs *args);

// =============================================================================
// BasicPattern.h
// =============================================================================

// =============================================================================
// Timer.h - defined here because needed by BasicPattern and BlinkStepPattern
// =============================================================================
typedef int8_t AlarmID;

#define ALARM_NONE -1
#define TIMER_FLAGS_NONE  0
#define TIMER_1_ALARM     1
#define TIMER_2_ALARMS    2
#define TIMER_3_ALARMS    3
#define TIMER_4_ALARMS    4
#define TIMER_ALARM_MASK  ( 1 | 2 | 3 | 4 )
#define TIMER_START       (1 << 7)

typedef struct Timer {
  uint32_t alarms[TIMER_4_ALARMS];
  uint8_t numAlarms;
  AlarmID curAlarm;
  uint32_t startTime;
} Timer;

void Timer_init(Timer *self);
void Timer_initFlags(Timer *self, uint8_t flags, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
AlarmID Timer_addAlarm(Timer *self, uint32_t interval);
void Timer_start(Timer *self, uint32_t offset);
void Timer_restart(Timer *self, uint32_t offset);
void Timer_reset(Timer *self);
AlarmID Timer_alarm(Timer *self);
bool Timer_onStart(const Timer *self);
bool Timer_onEnd(const Timer *self);
uint32_t Timer_numAlarms(const Timer *self);
uint32_t Timer_curAlarm(const Timer *self);
uint32_t Timer_startTime(const Timer *self);

typedef uint8_t PatternState;
enum {
  STATE_DISABLED,
  STATE_BLINK_ON,
  STATE_ON,
  STATE_BLINK_OFF,
  STATE_OFF,
  STATE_BEGIN_GAP,
  STATE_IN_GAP,
  STATE_BEGIN_DASH,
  STATE_IN_DASH,
  STATE_BEGIN_GAP2,
  STATE_IN_GAP2,
};

typedef struct {
  SingleLedPattern base;
  uint8_t onDuration;
  uint8_t offDuration;
  uint8_t gapDuration;
  uint8_t dashDuration;
  uint8_t groupSize;
  uint8_t groupCounter;
  uint8_t state;
  Timer blinkTimer;
} BasicPattern;

void BasicPattern_init(BasicPattern *self, const PatternArgs *args);
void BasicPattern_initVirtual(Pattern *self);
void BasicPattern_play(Pattern *self);
void BasicPattern_onBlinkOn(Pattern *self);
void BasicPattern_onBlinkOff(Pattern *self);
void BasicPattern_beginGap(Pattern *self);
void BasicPattern_beginDash(Pattern *self);
void BasicPattern_nextState(BasicPattern *self, uint8_t timing);

// =============================================================================
// SolidPattern.h
// =============================================================================
typedef struct {
  BasicPattern base;
  uint8_t colIndex;
} SolidPattern;

void SolidPattern_init(SolidPattern *self, const PatternArgs *args);
void SolidPattern_onBlinkOn(Pattern *self);

// =============================================================================
// BlendPattern.h
// =============================================================================
typedef struct {
  BasicPattern base;
  uint8_t blendSpeed;
  uint8_t numFlips;
  RGBColor cur;
  RGBColor next;
  uint8_t flip;
} BlendPattern;

void BlendPattern_init(BlendPattern *self, const PatternArgs *args);
void BlendPattern_initVirtual(Pattern *self);
void BlendPattern_onBlinkOn(Pattern *self);

// =============================================================================
// MultiLedPattern.h
// =============================================================================
typedef struct {
  Pattern base;
} MultiLedPattern;

void MultiLedPattern_init(MultiLedPattern *self, const PatternArgs *args);
void MultiLedPattern_bind(MultiLedPattern *self, LedPos pos);
void MultiLedPattern_initVirtual(MultiLedPattern *self);

// =============================================================================
// CompoundPattern.h
// =============================================================================
typedef struct {
  MultiLedPattern base;
  SingleLedPattern *ledPatterns[LED_COUNT];
} CompoundPattern;

void CompoundPattern_init(CompoundPattern *self, const PatternArgs *args);
void CompoundPattern_play(CompoundPattern *self);
void CompoundPattern_initVirtual(CompoundPattern *self);
void CompoundPattern_setPatternAtID(CompoundPattern *self, LedPos pos, PatternID id, const PatternArgs *args, const Colorset *set);
void CompoundPattern_setPatternAt(CompoundPattern *self, LedPos pos, SingleLedPattern *pat, const Colorset *set);
void CompoundPattern_clearPatterns(CompoundPattern *self);
void CompoundPattern_setEvensOdds(CompoundPattern *self, PatternID tipPat, PatternID topPat, const PatternArgs *tipArgs, const PatternArgs *topArgs);

// =============================================================================
// BlinkStepPattern.h
// =============================================================================
typedef struct {
  MultiLedPattern base;
  uint8_t blinkOnDuration;
  uint8_t blinkOffDuration;
  uint8_t stepDuration;
  Timer blinkTimer;
  Timer stepTimer;
} BlinkStepPattern;

void BlinkStepPattern_init(BlinkStepPattern *self, const PatternArgs *args);
void BlinkStepPattern_play(BlinkStepPattern *self);
void BlinkStepPattern_initVirtual(BlinkStepPattern *self);
void BlinkStepPattern_blinkOn(BlinkStepPattern *self);
void BlinkStepPattern_blinkOff(BlinkStepPattern *self);
void BlinkStepPattern_prestep(BlinkStepPattern *self);
void BlinkStepPattern_poststep(BlinkStepPattern *self);

// =============================================================================
// Sequence.h
// =============================================================================
typedef struct {
  PatternID m_patternMap[LED_COUNT];
} PatternMap;

void PatternMap_init(PatternMap *self);
void PatternMap_setPatternAt(PatternMap *self, PatternID pattern, LedMap positions);
PatternID PatternMap_getAt(const PatternMap *self, LedPos index);
bool PatternMap_serialize(const PatternMap *self, ByteStream *buffer);
bool PatternMap_unserialize(PatternMap *self, ByteStream *buffer);

typedef struct {
  Colorset colorsetMap[LED_COUNT];
} ColorsetMap;

void ColorsetMap_init(ColorsetMap *self);
void ColorsetMap_setColorsetAt(ColorsetMap *self, const Colorset *colorset, LedMap positions);
const Colorset *ColorsetMap_getAt(const ColorsetMap *self, LedPos index);
bool ColorsetMap_serialize(const ColorsetMap *self, ByteStream *buffer);
bool ColorsetMap_unserialize(ColorsetMap *self, ByteStream *buffer);

typedef struct {
  uint16_t duration;
  PatternMap patternMap;
  ColorsetMap colorsetMap;
} SequenceStep;

void SequenceStep_init(SequenceStep *self);
void SequenceStep_initFull(SequenceStep *self, uint16_t duration, const PatternMap *patternMap, const ColorsetMap *colorsetMap);
bool SequenceStep_serialize(const SequenceStep *self, ByteStream *buffer);
bool SequenceStep_unserialize(SequenceStep *self, ByteStream *buffer);

typedef struct {
  SequenceStep *sequenceSteps;
  uint8_t numSteps;
} Sequence;

void Sequence_init(Sequence *self);
void Sequence_destroy(Sequence *self);
void Sequence_copy(Sequence *self, const Sequence *other);
void Sequence_initSteps(Sequence *self, uint8_t numSteps);
uint8_t Sequence_addStep(Sequence *self, const SequenceStep *step);
void Sequence_clear(Sequence *self);
bool Sequence_serialize(const Sequence *self, ByteStream *buffer);
bool Sequence_unserialize(Sequence *self, ByteStream *buffer);
uint8_t Sequence_numSteps(const Sequence *self);
const SequenceStep *Sequence_getStep(const Sequence *self, uint8_t index);

// =============================================================================
// SequencedPattern.h
// =============================================================================
typedef struct {
  CompoundPattern base;
  Sequence sequence;
  uint8_t curSequence;
  Timer timer;
} SequencedPattern;

void SequencedPattern_init(SequencedPattern *self, const PatternArgs *args);
void SequencedPattern_initSeq(SequencedPattern *self, const PatternArgs *args, const Sequence *seq);
void SequencedPattern_play(SequencedPattern *self);
void SequencedPattern_initVirtual(SequencedPattern *self);
void SequencedPattern_bindSequence(SequencedPattern *self, const Sequence *seq);

// =============================================================================
// ChaserPattern.h (VORTEX_SLIM == 0 only)
// =============================================================================
typedef SequencedPattern ChaserPattern;
void ChaserPattern_init(ChaserPattern *self, const PatternArgs *args);

// Other multi-led pattern types (all VORTEX_SLIM == 0)
typedef struct {
  BlinkStepPattern base;
} TheaterChasePattern;
void TheaterChasePattern_init(TheaterChasePattern *self, const PatternArgs *args);
typedef BlinkStepPattern HueShiftPattern;
void HueShiftPattern_init(HueShiftPattern *self, const PatternArgs *args);
typedef BlinkStepPattern ZigzagPattern;
void ZigzagPattern_init(ZigzagPattern *self, const PatternArgs *args);
typedef BlinkStepPattern DripPattern;
void DripPattern_init(DripPattern *self, const PatternArgs *args);
typedef BlinkStepPattern DripMorphPattern;
void DripMorphPattern_init(DripMorphPattern *self, const PatternArgs *args);
typedef CompoundPattern CrossDopsPattern;
void CrossDopsPattern_init(CrossDopsPattern *self, const PatternArgs *args);
typedef BlinkStepPattern DoubleStrobePattern;
void DoubleStrobePattern_init(DoubleStrobePattern *self, const PatternArgs *args);
typedef BlinkStepPattern MeteorPattern;
void MeteorPattern_init(MeteorPattern *self, const PatternArgs *args);
typedef CompoundPattern SparkleTracePattern;
void SparkleTracePattern_init(SparkleTracePattern *self, const PatternArgs *args);
typedef BlinkStepPattern VortexWipePattern;
void VortexWipePattern_init(VortexWipePattern *self, const PatternArgs *args);
typedef BlinkStepPattern WarpPattern;
void WarpPattern_init(WarpPattern *self, const PatternArgs *args);
typedef CompoundPattern WarpWormPattern;
void WarpWormPattern_init(WarpWormPattern *self, const PatternArgs *args);
typedef BlinkStepPattern SnowballPattern;
void SnowballPattern_init(SnowballPattern *self, const PatternArgs *args);
typedef CompoundPattern LighthousePattern;
void LighthousePattern_init(LighthousePattern *self, const PatternArgs *args);
typedef BlinkStepPattern PulsishPattern;
void PulsishPattern_init(PulsishPattern *self, const PatternArgs *args);
typedef BlinkStepPattern FillPattern;
void FillPattern_init(FillPattern *self, const PatternArgs *args);
typedef CompoundPattern BouncePattern;
void BouncePattern_init(BouncePattern *self, const PatternArgs *args);
typedef BlinkStepPattern BackStrobePattern;
void BackStrobePattern_init(BackStrobePattern *self, const PatternArgs *args);
typedef BlinkStepPattern VortexPattern;
void VortexPattern_init(VortexPattern *self, const PatternArgs *args);

// =============================================================================
// ByteStream.h
// =============================================================================
typedef struct {
  uint32_t size;
  uint32_t flags;
  uint32_t crc32;
  uint8_t buf[];
} RawBuffer;

uint32_t RawBuffer_hash(const RawBuffer *self);
void RawBuffer_accumulate(RawBuffer *self, uint32_t val);
bool RawBuffer_verify(const RawBuffer *self);
void RawBuffer_recalcCRC(RawBuffer *self);

struct ByteStream {
  RawBuffer *pData;
  uint16_t position;
  uint16_t capacity;
};

bool ByteStream_init(ByteStream *self, uint32_t capacity, const uint8_t *buf);
void ByteStream_initStatic(ByteStream *self, uint8_t *storage, uint32_t storageSize);
void ByteStream_resetData(ByteStream *self);
void ByteStream_destroy(ByteStream *self);
void ByteStream_clear(ByteStream *self);
bool ByteStream_shrink(ByteStream *self);
bool ByteStream_append(ByteStream *self, const ByteStream *other);
bool ByteStream_extend(ByteStream *self, uint32_t size);
bool ByteStream_serialize8(ByteStream *self, uint8_t byte);
bool ByteStream_serialize16(ByteStream *self, uint16_t bytes);
bool ByteStream_serialize32(ByteStream *self, uint32_t bytes);
bool ByteStream_unserialize8(ByteStream *self, uint8_t *byte);
bool ByteStream_unserialize16(ByteStream *self, uint16_t *bytes);
bool ByteStream_unserialize32(ByteStream *self, uint32_t *bytes);
void ByteStream_resetUnserializer(ByteStream *self);
void ByteStream_moveUnserializer(ByteStream *self, uint32_t idx);
bool ByteStream_unserializerAtEnd(const ByteStream *self);
void ByteStream_sanity(ByteStream *self);
uint32_t ByteStream_size(const ByteStream *self);
uint32_t ByteStream_capacity(const ByteStream *self);
const uint8_t *ByteStream_data(const ByteStream *self);
void *ByteStream_rawData(const ByteStream *self);
uint16_t ByteStream_rawSize(const ByteStream *self);
bool ByteStream_compress(ByteStream *self);
bool ByteStream_decompress(ByteStream *self);
uint32_t ByteStream_recalcCRC(ByteStream *self, bool force);
bool ByteStream_checkCRC(const ByteStream *self);
bool ByteStream_isCRCDirty(const ByteStream *self);
void ByteStream_setCRCDirty(ByteStream *self);
bool ByteStream_is_compressed(const ByteStream *self);
uint32_t ByteStream_CRC(const ByteStream *self);
void ByteStream_trim(ByteStream *self, uint32_t bytes);
bool ByteStream_rawInit(ByteStream *self, const uint8_t *rawdata, uint32_t size);
void ByteStream_move(ByteStream *self, ByteStream *target);
uint8_t ByteStream_peek8(const ByteStream *self);
uint16_t ByteStream_peek16(const ByteStream *self);
uint32_t ByteStream_peek32(const ByteStream *self);
bool ByteStream_consume8(ByteStream *self, uint8_t *byte);
bool ByteStream_consume16(ByteStream *self, uint16_t *bytes);
bool ByteStream_consume32(ByteStream *self, uint32_t *bytes);
bool ByteStream_consume(ByteStream *self, uint32_t size, void *bytes);
uint8_t *ByteStream_frontUnserializer(const ByteStream *self);
uint8_t *ByteStream_frontSerializer(const ByteStream *self);
void ByteStream_copy(ByteStream *self, const ByteStream *other);

// =============================================================================
// BitStream.h
// =============================================================================
struct BitStream {
  uint8_t *buf;
  uint16_t buf_size;
  uint16_t bit_pos;
  bool buf_eof;
  bool allocated;
};

void BitStream_init(BitStream *self);
void BitStream_initBuf(BitStream *self, uint8_t *buf, uint32_t size);
bool BitStream_initAlloc(BitStream *self, uint32_t size);
void BitStream_destroy(BitStream *self);
void BitStream_reset(BitStream *self);
void BitStream_resetPos(BitStream *self);
uint8_t BitStream_read1Bit(BitStream *self);
void BitStream_write1Bit(BitStream *self, bool bit);
uint8_t BitStream_readBits(BitStream *self, uint32_t numBits);
void BitStream_writeBits(BitStream *self, uint32_t numBits, uint32_t val);
bool BitStream_eof(const BitStream *self);
bool BitStream_allocated(const BitStream *self);
uint16_t BitStream_size(const BitStream *self);
const uint8_t *BitStream_data(const BitStream *self);
uint8_t BitStream_peekData(const BitStream *self, uint8_t pos);
uint16_t BitStream_dwordpos(const BitStream *self);
uint16_t BitStream_bytepos(const BitStream *self);
uint16_t BitStream_bitpos(const BitStream *self);

// =============================================================================
// Compression.h
// =============================================================================
#if VORTEX_SLIM == 1
#define compress_size(srcSize) 0
#define compress_buffer(src, dst, srcSize, dstCapacity)
#define decompress_buffer(src, dst, srcSize, dstCapacity)
#else
#define compress_size(srcSize) LZ4_compressBound(srcSize)
#define compress_buffer(src, dst, srcSize, dstCapacity) LZ4_compress_default((const char *)(src), (char *)(dst), (srcSize), (dstCapacity))
#define decompress_buffer(src, dst, srcSize, dstCapacity) LZ4_decompress_safe((const char *)(src), (char *)(dst), (srcSize), (dstCapacity))
#define LZ4_MEMORY_USAGE_MIN 10
#define LZ4_MEMORY_USAGE_DEFAULT 10
#define LZ4_MEMORY_USAGE_MAX 20
#ifndef LZ4_MEMORY_USAGE
#define LZ4_MEMORY_USAGE LZ4_MEMORY_USAGE_DEFAULT
#endif
#define LZ4_MAX_INPUT_SIZE        0x7E000000
#define LZ4_COMPRESSBOUND(isize)  ((unsigned)(isize) > (unsigned)LZ4_MAX_INPUT_SIZE ? 0 : (isize) + ((isize)/255) + 16)
#define LZ4_HASHLOG   (LZ4_MEMORY_USAGE-2)
#define LZ4_HASHTABLESIZE (1 << LZ4_MEMORY_USAGE)
#define LZ4_HASH_SIZE_U32 (1 << LZ4_HASHLOG)
#define LZ4_STREAMSIZE       ((1UL << LZ4_MEMORY_USAGE) + 32)
#define LZ4_STREAMSIZE_VOIDP (LZ4_STREAMSIZE / sizeof(void*))
#define LZ4_DECODER_RING_BUFFER_SIZE(maxBlockSize) (65536 + 14 + (maxBlockSize))
#define LZ4_DECOMPRESS_INPLACE_MARGIN(compressedSize)          (((compressedSize) >> 8) + 32)
#define LZ4_DECOMPRESS_INPLACE_BUFFER_SIZE(decompressedSize)   ((decompressedSize) + LZ4_DECOMPRESS_INPLACE_MARGIN(decompressedSize))
#define LZ4_DISTANCE_MAX 65535
#define LZ4_COMPRESS_INPLACE_MARGIN                           (LZ4_DISTANCE_MAX + 32)
#define LZ4_COMPRESS_INPLACE_BUFFER_SIZE(maxCompressedSize)   ((maxCompressedSize) + LZ4_COMPRESS_INPLACE_MARGIN)
typedef int8_t LZ4_i8;
typedef uint8_t LZ4_byte;
typedef uint16_t LZ4_u16;
typedef uint32_t LZ4_u32;
typedef struct LZ4_stream_t_internal LZ4_stream_t_internal;
struct LZ4_stream_t_internal {
  LZ4_u32 hashTable[LZ4_HASH_SIZE_U32];
  LZ4_u32 currentOffset;
  LZ4_u32 tableType;
  const LZ4_byte *dictionary;
  const LZ4_stream_t_internal *dictCtx;
  LZ4_u32 dictSize;
};
typedef struct {
  const LZ4_byte *externalDict;
  size_t extDictSize;
  const LZ4_byte *prefixEnd;
  size_t prefixSize;
} LZ4_streamDecode_t_internal;
typedef union LZ4_stream_u LZ4_stream_t;
union LZ4_stream_u {
  void *table[LZ4_STREAMSIZE_VOIDP];
  LZ4_stream_t_internal internal_donotuse;
};
typedef union LZ4_streamDecode_u LZ4_streamDecode_t;
union LZ4_streamDecode_u {
  unsigned long long table[LZ4_STREAMDECODESIZE_U64];
  LZ4_streamDecode_t_internal internal_donotuse;
};
#define LZ4_STREAMDECODESIZE_U64 (4 + ((sizeof(void*)==16) ? 2 : 0))
#define LZ4_STREAMDECODESIZE     (LZ4_STREAMDECODESIZE_U64 * sizeof(unsigned long long))
int LZ4_compress_default(const char *src, char *dst, int srcSize, int dstCapacity);
int LZ4_decompress_safe(const char *src, char *dst, int compressedSize, int dstCapacity);
int LZ4_compressBound(int inputSize);
int LZ4_compress_fast(const char *src, char *dst, int srcSize, int dstCapacity, int acceleration);
int LZ4_compress_fast_extState(void *state, const char *src, char *dst, int srcSize, int dstCapacity, int acceleration);
LZ4_stream_t *LZ4_initStream(void *buffer, size_t size);
#endif

// =============================================================================
// Button.h
// =============================================================================
typedef struct {
  uint32_t pressTime;
  uint32_t releaseTime;
  uint32_t holdDuration;
  uint32_t releaseDuration;
  uint8_t consecutivePresses;
  uint8_t releaseCount;
  bool buttonState;
  bool newPress;
  bool newRelease;
  bool isPressed;
  bool shortClick;
  bool longClick;
} Button;

void Button_init(Button *self);
bool Button_check(Button *self);
void Button_update(Button *self);
bool Button_onPress(const Button *self);
bool Button_onRelease(const Button *self);
bool Button_isPressed(const Button *self);
bool Button_onShortClick(const Button *self);
bool Button_onLongClick(const Button *self);
bool Button_onConsecutivePresses(Button *self, uint8_t num);
uint32_t Button_holdDuration(const Button *self);
uint32_t Button_releaseDuration(const Button *self);
uint32_t Button_pressTime(const Button *self);
uint32_t Button_releaseTime(const Button *self);
uint8_t Button_consecutivePresses(const Button *self);
uint8_t Button_releaseCount(const Button *self);
void Button_enableWake(Button *self);

extern Button *g_pButton;

// =============================================================================
// Buttons.h
// =============================================================================
#define NUM_BUTTONS 1

bool Buttons_init(uint8_t pin);
void Buttons_cleanup(void);
void Buttons_update(void);

// =============================================================================
// Leds.h
// =============================================================================
bool Leds_init(void);
void Leds_cleanup(void);
void Leds_setIndex(LedPos target, RGBColor col);
void Leds_setRange(LedPos first, LedPos last, RGBColor col);
void Leds_setAll(RGBColor col);
void Leds_clearIndex(LedPos target);
void Leds_clearRange(LedPos first, LedPos last);
void Leds_clearAll(void);
void Leds_setPair(Pair pair, RGBColor col);
void Leds_setPairs(Pair first, Pair last, RGBColor col);
void Leds_clearPair(Pair pair);
void Leds_clearPairs(Pair first, Pair last);
void Leds_setRangeEvens(Pair first, Pair last, RGBColor col);
void Leds_setAllEvens(RGBColor col);
void Leds_setRangeOdds(Pair first, Pair last, RGBColor col);
void Leds_setAllOdds(RGBColor col);
void Leds_clearRangeEvens(Pair first, Pair last);
void Leds_clearAllEvens(void);
void Leds_clearRangeOdds(Pair first, Pair last);
void Leds_clearAllOdds(void);
void Leds_setMap(LedMap map, RGBColor col);
void Leds_clearMap(LedMap map);
void Leds_stashAll(LedStash *stash);
void Leds_restoreAll(const LedStash *stash);
void Leds_adjustBrightnessIndex(LedPos target, uint8_t fadeBy);
void Leds_adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy);
void Leds_adjustBrightnessAll(uint8_t fadeBy);
void Leds_blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkRangeOffset(LedPos first, LedPos last, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkIndex(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkRange(LedPos first, LedPos last, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkMap(LedMap targets, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkAll(uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkPair(Pair pair, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_blinkPairs(Pair first, Pair last, uint16_t offMs, uint16_t onMs, RGBColor col);
void Leds_breatheIndex(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheRange(LedPos first, LedPos last, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheIndexSat(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_breatheIndexVal(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val);
void Leds_holdAll(RGBColor col);
RGBColor Leds_getLed(LedPos pos);
uint8_t Leds_getBrightness(void);
void Leds_setBrightness(uint8_t brightness);
void Leds_update(void);

extern uint8_t Leds_brightness;
extern RGBColor Leds_ledColors[LED_COUNT];

// =============================================================================
// LedStash.h
// =============================================================================
struct LedStash {
  RGBColor ledColorsStash[LED_COUNT];
};

void LedStash_init(LedStash *self);
void LedStash_setIndex(LedStash *self, LedPos pos, RGBColor col);
void LedStash_clear(LedStash *self);

// =============================================================================
// TimeControl.h
// =============================================================================
#if VARIABLE_TICKRATE == 1
#define MS_TO_TICKS(ms) Time_millisecondsToTicks(ms)
#define SEC_TO_TICKS(s) Time_secondsToTicks(s)
#else
#define MS_TO_TICKS(ms) (uint32_t)(((uint32_t)(ms) * DEFAULT_TICKRATE) / 1000)
#define SEC_TO_TICKS(s) (uint32_t)((uint32_t)(s) * DEFAULT_TICKRATE)
#endif

#ifdef VORTEX_LIB
#define SIMULATION_TICK getSimulationTick()
#else
#define SIMULATION_TICK 0
#endif

bool Time_init(void);
void Time_cleanup(void);
void Time_tickClock(void);
uint32_t Time_getCurtime(void);
uint32_t Time_getRealCurtime(void);
void Time_setTickrate(uint32_t tickrate);
uint32_t Time_getTickrate(void);
#if VARIABLE_TICKRATE == 1
uint32_t Time_millisecondsToTicks(uint32_t ms);
uint32_t Time_secondsToTicks(uint32_t sec);
#endif
uint32_t Time_microseconds(void);
void Time_delayMicroseconds(uint32_t us);
void Time_delayMilliseconds(uint32_t ms);
#ifdef VORTEX_LIB
uint32_t Time_startSimulation(void);
uint32_t Time_tickSimulation(void);
bool Time_isSimulation(void);
uint32_t Time_getSimulationTick(void);
uint32_t Time_endSimulation(void);
void Time_setInstantTimestep(bool instant);
bool Time_isInstantStepping(void);
#endif

// =============================================================================
// Timings.h
// =============================================================================
#define MENU_TRIGGER_THRESHOLD_TICKS  MS_TO_TICKS(MENU_TRIGGER_TIME)
#define SHORT_CLICK_THRESHOLD_TICKS   MS_TO_TICKS(CLICK_THRESHOLD)
#define CONSECUTIVE_WINDOW_TICKS      MS_TO_TICKS(CONSECUTIVE_WINDOW)
#define AUTO_RANDOM_DELAY_TICKS       MS_TO_TICKS(AUTO_RANDOM_DELAY)
#define SLEEP_ENTER_THRESHOLD_TICKS   MS_TO_TICKS(SLEEP_TRIGGER_TIME)
#define SLEEP_WINDOW_THRESHOLD_TICKS  MS_TO_TICKS(SLEEP_WINDOW_TIME)
#define FORCE_SLEEP_THRESHOLD_TICKS   MS_TO_TICKS(FORCE_SLEEP_TIME)
#define ONE_CLICK_THRESHOLD_TICKS     MS_TO_TICKS(ONE_CLICK_MODE_TRHESHOLD)
#define DELETE_THRESHOLD_TICKS        MS_TO_TICKS(COL_DELETE_THRESHOLD)
#define DELETE_CYCLE_TICKS            MS_TO_TICKS(COL_DELETE_CYCLE)
#define FACTORY_RESET_THRESHOLD_TICKS MS_TO_TICKS(RESET_HOLD_TIME)
#define MAX_SERIAL_CHECK_INTERVAL     MS_TO_TICKS(SERIAL_CHECK_TIME)
#define MAX_TIMEOUT_DURATION          MS_TO_TICKS(IR_RECEIVER_TIMEOUT_DURATION)
#define MAX_WAIT_DURATION             MS_TO_TICKS(IR_SENDER_WAIT_DURATION)
#define ADV_MENU_DURATION_TICKS       MS_TO_TICKS(ADVANCED_MENU_ENTER_DURATION)
#ifdef UNLOCK_WAKE_WINDOW
#define UNLOCK_WAKE_WINDOW_TICKS      MS_TO_TICKS(UNLOCK_WAKE_WINDOW)
#endif

#define STROBE_ON_DURATION          6
#define STROBE_OFF_DURATION         6
#define HYPERSTROBE_ON_DURATION     16
#define HYPERSTROBE_OFF_DURATION    20
#define PICOSTROBE_ON_DURATION      6
#define PICOSTROBE_OFF_DURATION     40
#define DOPS_ON_DURATION            1
#define DOPS_OFF_DURATION           10
#define ULTRADOPS_ON_DURATION       1
#define ULTRADOPS_OFF_DURATION      3
#define STROBIE_ON_DURATION         2
#define STROBIE_OFF_DURATION        28
#define SIGNAL_ON_DURATION          10
#define SIGNAL_OFF_DURATION         296
#define BLEND_ON_DURATION           2
#define BLEND_OFF_DURATION          13
#define RIBBON_DURATION             6
#define BULB_SELECT_ON_MS           150
#define BULB_SELECT_OFF_MS          50
#define EXIT_MENU_ON_MS             300
#define EXIT_MENU_OFF_MS            150

// =============================================================================
// Mode.h
// =============================================================================
typedef uint8_t ModeFlags;

#define MODE_FLAG_NONE              0
#define MODE_FLAG_MULTI_LED         (1 << 0)
#define MODE_FLAG_SINGLE_LED        (1 << 1)
#define MODE_FLAG_ALL_SAME_SINGLE   (1 << 2)
#define MODE_FLAG_SPARSE_SINGLES    (1 << 3)

struct Mode {
#if VORTEX_SLIM == 0
  Pattern *m_multiPat;
#endif
#if FIXED_LED_COUNT == 0
  uint8_t m_numLeds;
  Pattern **m_singlePats;
#else
  Pattern *m_singlePats[LED_COUNT];
#endif
};

void Mode_init(Mode *self);
void Mode_initFromID(Mode *self, PatternID id, const Colorset *set);
void Mode_initMode(Mode *self);
void Mode_initModeArgs(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set);
void Mode_initFull(Mode *self, PatternID id, const Colorset *set);
Colorset Mode_getColorsetMut(Mode *self, LedPos pos);
void Mode_destroy(Mode *self);
void Mode_assign(Mode *self, const Mode *other);
void Mode_copy(Mode *self, const Mode *other);
bool Mode_equals(const Mode *self, const Mode *other);
void Mode_initVirtual(Mode *self);
void Mode_play(Mode *self);
bool Mode_saveToBuffer(const Mode *self, ByteStream *saveBuffer, uint8_t numLeds);
bool Mode_loadFromBuffer(Mode *self, ByteStream *saveBuffer);
bool Mode_serialize(const Mode *self, ByteStream *buffer, uint8_t numLeds);
bool Mode_unserialize(Mode *self, ByteStream *buffer);
bool Mode_setPattern(Mode *self, PatternID pat, LedPos pos, const PatternArgs *args, const Colorset *set);
bool Mode_setPatternMap(Mode *self, LedMap pos, PatternID pat, const PatternArgs *args, const Colorset *set);
void Mode_copyPatternFrom(Mode *self, const Mode *other, LedPos to, LedPos from);
void Mode_swapPatterns(Mode *self, LedPos a, LedPos b);
bool Mode_setColorset(Mode *self, const Colorset *set, LedPos pos);
bool Mode_setColorsetMap(Mode *self, LedMap map, const Colorset *set);
void Mode_clearPattern(Mode *self, LedPos pos);
void Mode_clearPatternMap(Mode *self, LedMap map);
void Mode_clearColorset(Mode *self, LedPos pos);
void Mode_clearColorsetMap(Mode *self, LedMap map);
void Mode_setArg(Mode *self, uint8_t index, uint8_t value, LedMap map);
uint8_t Mode_getArg(Mode *self, uint8_t index, LedPos pos);
ModeFlags Mode_getFlags(const Mode *self);
bool Mode_hasMultiLed(const Mode *self);
bool Mode_hasSingleLed(const Mode *self);
bool Mode_hasSameSingleLed(const Mode *self);
bool Mode_hasSparseSingleLed(const Mode *self);
bool Mode_isEmpty(const Mode *self);
LedMap Mode_getSingleLedMap(const Mode *self);
bool Mode_isMultiLed(const Mode *self);
uint8_t Mode_getLedCount(const Mode *self);
Pattern *Mode_getPattern(Mode *self, LedPos pos);
const Pattern *Mode_getPatternConst(const Mode *self, LedPos pos);
Colorset Mode_getColorset(const Mode *self, LedPos pos);
PatternID Mode_getPatternID(const Mode *self, LedPos pos);
#if FIXED_LED_COUNT == 0
bool Mode_setLedCount(Mode *self, uint8_t numLeds);
#endif

// =============================================================================
// Modes.h
// =============================================================================
#define MODES_FLAG_LOCKED     (1 << 0)
#define MODES_FLAG_ONE_CLICK  (1 << 1)
#define MODES_FLAG_ADV_MENUS  (1 << 2)
#define MODES_FLAG_KEYCHAIN   (1 << 3)
#define MODES_FLAG_NEW_FIRMWARE 0xF0

bool Modes_init(void);
void Modes_cleanup(void);
bool Modes_load(void);
void Modes_play(void);
bool Modes_saveToBuffer(ByteStream *saveBuffer);
bool Modes_loadFromBuffer(ByteStream *saveBuffer);
bool Modes_saveHeader(void);
bool Modes_loadHeader(void);
bool Modes_saveHeaderAndMode(void);
bool Modes_saveStorage(void);
bool Modes_loadStorage(void);
bool Modes_serializeSaveHeader(ByteStream *saveBuffer);
bool Modes_unserializeSaveHeader(ByteStream *saveBuffer);
bool Modes_serialize(ByteStream *buffer);
bool Modes_unserialize(ByteStream *buffer);
bool Modes_setDefaults(void);
bool Modes_shiftCurMode(int32_t offset);
bool Modes_addMode(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
bool Modes_addModeArgs(PatternID id, const PatternArgs *args, const Colorset *set);
bool Modes_addModeMode(const Mode *mode);
bool Modes_addSerializedMode(ByteStream *serializedMode);
bool Modes_addModeFromBuffer(ByteStream *serializedMode);
bool Modes_updateCurMode(const Mode *mode);
Mode *Modes_setCurMode(uint8_t index);
Mode *Modes_curMode(void);
Mode *Modes_nextMode(void);
Mode *Modes_previousMode(void);
Mode *Modes_nextModeSkipEmpty(void);
uint8_t Modes_numModes(void);
uint8_t Modes_curModeIndex(void);
uint32_t Modes_lastSwitchTime(void);
void Modes_deleteCurMode(void);
void Modes_clearModes(void);
void Modes_setStartupMode(uint8_t index);
uint8_t Modes_startupMode(void);
Mode *Modes_switchToStartupMode(void);
bool Modes_setFlag(uint8_t flag, bool enable, bool save);
bool Modes_getFlag(uint8_t flag);
void Modes_resetFlags(void);
bool Modes_setOneClickMode(bool enable, bool save);
bool Modes_oneClickModeEnabled(void);
bool Modes_setLocked(bool locked, bool save);
bool Modes_locked(void);
bool Modes_setAdvancedMenus(bool active, bool save);
bool Modes_advancedMenusEnabled(void);
bool Modes_setKeychainMode(bool active, bool save);
bool Modes_keychainModeEnabled(void);

// =============================================================================
// DefaultModes.h
// =============================================================================
struct DefaultLedEntry {
  PatternID patternID;
  uint8_t numColors;
  const uint32_t *cols;
};

struct DefaultModeEntry {
  struct DefaultLedEntry leds[LED_COUNT];
};

extern const struct DefaultModeEntry defaultModes[MAX_MODES];

// =============================================================================
// Serial.h
// =============================================================================
bool SerialComs_init(void);
void SerialComs_cleanup(void);
bool SerialComs_isConnected(void);
bool SerialComs_checkSerial(void);
void SerialComs_write(const char *msg, ...);
void SerialComs_writeStream(ByteStream *byteStream);
void SerialComs_read(ByteStream *byteStream);
bool SerialComs_dataReady(void);

// =============================================================================
// Storage.h
// =============================================================================
bool Storage_init(void);
void Storage_cleanup(void);
bool Storage_write(uint8_t slot, ByteStream *buffer);
bool Storage_read(uint8_t slot, ByteStream *buffer);
uint32_t Storage_lastSaveSize(void);
#ifdef VORTEX_LIB
void Storage_setStorageFilename(const char *name);
const char *Storage_getStorageFilename(void);
#endif

// =============================================================================
// Wireless headers
// =============================================================================

// IRSender
bool IRSender_init(void);
void IRSender_cleanup(void);
bool IRSender_loadMode(const Mode *targetMode);
bool IRSender_send(void);
bool IRSender_isSending(void);

// IRReceiver
bool IRReceiver_init(void);
void IRReceiver_cleanup(void);
bool IRReceiver_dataReady(void);
bool IRReceiver_isReceiving(void);
uint8_t IRReceiver_percentReceived(void);
bool IRReceiver_receiveMode(Mode *pMode);
bool IRReceiver_beginReceiving(void);
bool IRReceiver_endReceiving(void);
bool IRReceiver_onNewData(void);
void IRReceiver_resetIRState(void);

// VLSender
bool VLSender_init(void);
void VLSender_cleanup(void);
void VLSender_send(const Mode *targetMode);
void VLSender_sendLegacy(const Mode *targetMode);
void VLSender_sendMarkSpace(uint16_t markTime, uint16_t spaceTime);
void VLSender_sendByte(uint8_t data);
void VLSender_sendByteLegacy(uint8_t data);
void VLSender_startPWM(void);
void VLSender_stopPWM(void);

// VLReceiver
bool VLReceiver_init(void);
void VLReceiver_cleanup(void);
bool VLReceiver_dataReady(void);
bool VLReceiver_isReceiving(void);
uint8_t VLReceiver_percentReceived(void);
bool VLReceiver_receiveMode(Mode *pMode);
bool VLReceiver_beginReceiving(void);
bool VLReceiver_endReceiving(void);
bool VLReceiver_onNewData(void);
void VLReceiver_resetVLState(void);
void VLReceiver_setLegacyReceiver(bool legacy);
void VLReceiver_recvPCIHandler(void);
bool VLReceiver_read(ByteStream *data);
void VLReceiver_handleVLTiming(uint16_t diff);
void VLReceiver_handleVLTimingLegacy(uint16_t diff);

// =============================================================================
// VortexEngine.h
// =============================================================================
bool VortexEngine_init(void);
void VortexEngine_cleanup(void);
void VortexEngine_tick(void);
void VortexEngine_runMainLogic(void);
bool VortexEngine_serializeVersion(ByteStream *stream);
bool VortexEngine_checkVersion(uint8_t major, uint8_t minor);
Mode *VortexEngine_curMode(void);
void VortexEngine_enterSleep(bool save);
void VortexEngine_wakeup(bool reset);
void VortexEngine_toggleForceSleep(bool enabled);
void VortexEngine_setAutoCycle(bool enabled);

// =============================================================================
// PatternBuilder.h
// =============================================================================
#define MAX_PATTERN_POOL 6

typedef union {
  Pattern base;
  SingleLedPattern single;
  BasicPattern basic;
  BlendPattern blend;
  SolidPattern solid;
} PatternUnion;

Pattern *PatternBuilder_make(PatternID id, const PatternArgs *args);
Pattern *PatternBuilder_dupe(const Pattern *pat);
Pattern *PatternBuilder_makeSingle(PatternID id, const PatternArgs *args);
Pattern *PatternBuilder_unserialize(ByteStream *buffer);
PatternArgs PatternBuilder_getDefaultArgs(PatternID id);
uint8_t PatternBuilder_numDefaultArgs(PatternID id);
Pattern *PatternBuilder_alloc(void);
void PatternBuilder_release(Pattern *pat);

// =============================================================================
// Log.h
// =============================================================================
#if LOGGING_LEVEL > 0
#define INFO_LOG(msg) InfoMsg(msg)
#define INFO_LOGF(msg, ...) InfoMsg(msg, __VA_ARGS__)
void InfoMsg(const char *msg, ...);
#else
#define INFO_LOG(msg)
#define INFO_LOGF(msg, ...)
#endif
#if LOGGING_LEVEL > 1
#define ERROR_LOG(msg) ErrorMsg(__FUNCTION__, msg)
#define ERROR_LOGF(msg, ...) ErrorMsg(__FUNCTION__, msg, __VA_ARGS__)
void ErrorMsg(const char *func, const char *msg, ...);
#else
#define ERROR_LOG(msg)
#define ERROR_LOGF(msg, ...)
#endif
#if LOGGING_LEVEL > 2
#define DEBUG_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define DEBUG_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
void DebugMsg(const char *file, const char *func, int line, const char *msg, ...);
#else
#define DEBUG_LOG(msg)
#define DEBUG_LOGF(msg, ...)
#endif
#define ERROR_OUT_OF_MEMORY() ERROR_LOG("Out of memory")

// =============================================================================
// Memory.h
// =============================================================================
#if DEBUG_ALLOCATIONS == 1
#define vmalloc(size) _vmalloc(size)
#define vcalloc(size, amount) _vcalloc(size, amount)
#define vrealloc(ptr, size) _vrealloc(ptr, size)
#define vfree(ptr) _vfree(ptr)
void *_vmalloc(uint32_t size);
void *_vcalloc(uint32_t size, uint32_t amount);
void *_vrealloc(void *ptr, uint32_t size);
void _vfree(void *ptr);
uint32_t cur_memory_usage(void);
uint32_t cur_memory_usage_background(void);
uint32_t cur_memory_usage_total(void);
#else
#define vmalloc(size) malloc(size)
#define vcalloc(size, amount) calloc(size, amount)
#define vrealloc(ptr, size) realloc(ptr, size)
#define vfree(ptr) free(ptr)
#endif

// =============================================================================
// Menu.h
// =============================================================================

typedef uint8_t MenuAction;
enum {
  MENU_QUIT,
  MENU_CONTINUE,
  MENU_SKIP
};

typedef struct MenuVTable MenuVTable;
struct MenuVTable {
  void (*destroy)(Menu *self);
  bool (*init)(Menu *self);
  MenuAction (*run)(Menu *self);
  void (*onLedSelected)(Menu *self);
  void (*onShortClick)(Menu *self);
  void (*onLongClick)(Menu *self);
  void (*leaveMenu)(Menu *self, bool doSave);
};

struct Menu {
  const MenuVTable *vtable;
  Mode previewMode;
  RGBColor menuColor;
  LedMap targetLeds;
  uint8_t curSelection;
  bool ledSelected;
  bool advanced;
  bool shouldClose;
};

void Menu_construct(Menu *self, RGBColor col, bool advanced);
void Menu_destroy(Menu *self);
bool Menu_init(Menu *self);
MenuAction Menu_run(Menu *self);
void Menu_onLedSelected(Menu *self);
void Menu_onShortClick(Menu *self);
void Menu_onLongClick(Menu *self);
void Menu_leaveMenu(Menu *self, bool doSave);
void Menu_showBulbSelection(Menu *self);
void Menu_showExit(Menu *self);
void Menu_nextBulbSelection(Menu *self);
void Menu_bypassLedSelection(Menu *self, LedMap map);

// =============================================================================
// Menus.h
// =============================================================================

enum MenuEntryID {
  MENU_NONE = -1,
  MENU_FIRST = 0,
  MENU_RANDOMIZER = MENU_FIRST,
  MENU_MODE_SHARING,
#if ENABLE_EDITOR_CONNECTION == 1
  MENU_EDITOR_CONNECTION,
#endif
  MENU_COLOR_SELECT,
  MENU_PATTERN_SELECT,
  MENU_GLOBAL_BRIGHTNESS,
  MENU_FACTORY_RESET,
  MENU_COUNT
};

bool Menus_init(void);
void Menus_cleanup(void);
bool Menus_run(void);
bool Menus_openMenuSelection(void);
bool Menus_openMenu(uint32_t index, bool advanced);
void Menus_showSelection(RGBColor colval);
bool Menus_checkOpen(void);
bool Menus_checkInMenu(void);
Menu *Menus_curMenu(void);
enum MenuEntryID Menus_curMenuID(void);

// =============================================================================
// Menu List type forward declarations and constructors
// =============================================================================

typedef struct {
  Menu base;
  uint8_t state;
  HSVColor newColor;
  Colorset colorset;
  uint8_t targetSlot;
  uint8_t targetHue1;
} ColorSelectMenu;

typedef struct {
  Menu base;
  LedPos srcLed;
  bool started;
} PatternSelectMenu;

typedef struct {
  Menu base;
#if FIXED_LED_COUNT == 0
  Random_t singlesRandCtx[];
#else
  Random_t singlesRandCtx[LED_COUNT];
#endif
#if VORTEX_SLIM == 0
  Random_t multiRandCtx;
#endif
  uint32_t lastRandomization;
  uint8_t flags;
  uint8_t displayHue;
  bool needToSelect;
  bool autoCycle;
} RandomizerMenu;

typedef struct {
  Menu base;
  uint8_t brightnessOptions[4];
  uint8_t keychainModeState;
  uint32_t lastStateChange;
  uint8_t colorIndex;
} GlobalBrightnessMenu;

typedef struct {
  Menu base;
} FactoryResetMenu;

typedef struct {
  Menu base;
  ByteStream receiveBuffer;
  uint8_t state;
  bool allowReset;
  uint8_t previousModeIndex;
  uint8_t numModesToReceive;
  uint8_t rv;
} EditorConnectionMenu;

typedef struct {
  Menu base;
  uint8_t sharingMode;
  uint32_t timeOutStartTime;
  uint32_t lastPercentChange;
  uint8_t lastPercent;
} ModeSharingMenu;

Menu *ColorSelectMenu_Create(RGBColor col, bool advanced);
Menu *PatternSelectMenu_Create(RGBColor col, bool advanced);
Menu *RandomizerMenu_Create(RGBColor col, bool advanced);
Menu *GlobalBrightnessMenu_Create(RGBColor col, bool advanced);
Menu *FactoryResetMenu_Create(RGBColor col, bool advanced);
Menu *EditorConnectionMenu_Create(RGBColor col, bool advanced);
Menu *ModeSharingMenu_Create(RGBColor col, bool advanced);

typedef union {
  ColorSelectMenu colorSelect;
  PatternSelectMenu patternSelect;
  RandomizerMenu randomizer;
  GlobalBrightnessMenu globalBrightness;
  FactoryResetMenu factoryReset;
  EditorConnectionMenu editorConnection;
  ModeSharingMenu modeSharing;
} MenuWorkspace;

extern MenuWorkspace g_menuWorkspace;

extern const MenuVTable g_menuVTable;

// =============================================================================
// IRConfig.h and VLConfig.h (used by some .c files)
// =============================================================================
#define VL_ENABLE_SENDER          1
#define VL_ENABLE_RECEIVER        1
#define VL_DEFAULT_BLOCK_SIZE 256
#define VL_DEFAULT_BLOCK_SPACING MS_TO_TICKS(5)
#define VL_MAX_DWORDS_TRANSFER 128
#define VL_MAX_DATA_TRANSFER (VL_MAX_DWORDS_TRANSFER * sizeof(uint32_t))
#define VL_RECV_BUF_SIZE VL_MAX_DATA_TRANSFER
#define VL_THRESHOLD  0.5
#define VL_THRES_UP   (1 + VL_THRESHOLD)
#define VL_THRES_DOWN (1 - VL_THRESHOLD)
#define VL_TIMING (uint16_t)(2230)
#define VL_HEADER_MARK (uint16_t)(VL_TIMING * 16)
#define VL_HEADER_SPACE (uint16_t)(VL_TIMING * 8)
#define VL_HEADER_MARK_MIN ((uint16_t)(VL_HEADER_MARK * VL_THRES_DOWN))
#define VL_HEADER_SPACE_MIN ((uint16_t)(VL_HEADER_SPACE * VL_THRES_DOWN))
#define VL_HEADER_MARK_MAX ((uint16_t)(VL_HEADER_MARK * VL_THRES_UP))
#define VL_HEADER_SPACE_MAX ((uint16_t)(VL_HEADER_SPACE * VL_THRES_UP))
#define VL_TIMING_BIT_ONE (uint16_t)(VL_TIMING * 3)
#define VL_TIMING_BIT_ZERO (uint16_t)(VL_TIMING)
#define VL_TIMING_BIT(bit) ((bit) ? VL_TIMING_BIT_ONE : VL_TIMING_BIT_ZERO)
#define VL_TIMING_LEGACY (uint16_t)(3230)
#define VL_HEADER_MARK_LEGACY (uint16_t)(VL_TIMING_LEGACY * 16)
#define VL_HEADER_SPACE_LEGACY (uint16_t)(VL_TIMING_LEGACY * 8)
#define VL_HEADER_MARK_MIN_LEGACY ((uint16_t)(VL_HEADER_MARK_LEGACY * VL_THRES_DOWN))
#define VL_HEADER_SPACE_MIN_LEGACY ((uint16_t)(VL_HEADER_SPACE_LEGACY * VL_THRES_DOWN))
#define VL_HEADER_MARK_MAX_LEGACY ((uint16_t)(VL_HEADER_MARK_LEGACY * VL_THRES_UP))
#define VL_HEADER_SPACE_MAX_LEGACY ((uint16_t)(VL_HEADER_SPACE_LEGACY * VL_THRES_UP))
#define VL_TIMING_BIT_ONE_LEGACY (uint16_t)(VL_TIMING_LEGACY * 3)
#define VL_TIMING_BIT_ZERO_LEGACY (uint16_t)(VL_TIMING_LEGACY)
#define VL_TIMING_BIT_LEGACY(bit) ((bit) ? VL_TIMING_BIT_ONE_LEGACY : VL_TIMING_BIT_ZERO_LEGACY)
#define VL_SEND_PWM_PIN 0
#define VL_RECEIVER_PIN 0

#define IR_ENABLE_SENDER          0
#define IR_ENABLE_RECEIVER        0
#define IR_DEFAULT_BLOCK_SIZE 32
#define IR_DEFAULT_BLOCK_SPACING MS_TO_TICKS(300)
#define IR_MAX_DWORDS_TRANSFER 1024
#define IR_MAX_DATA_TRANSFER (IR_MAX_DWORDS_TRANSFER * sizeof(uint32_t))
#define IR_RECV_BUF_SIZE IR_MAX_DATA_TRANSFER
#define IR_THRESHOLD  0.25
#define IR_THRES_UP   (1 + IR_THRESHOLD)
#define IR_THRES_DOWN (1 - IR_THRESHOLD)
#define IR_TIMING (uint32_t)562
#define IR_TIMING_MIN ((uint32_t)(IR_TIMING * IR_THRES_DOWN))
#define IR_HEADER_MARK (uint32_t)(IR_TIMING * 16)
#define IR_HEADER_SPACE (uint32_t)(IR_TIMING * 8)
#define IR_HEADER_MARK_MIN ((uint32_t)(IR_HEADER_MARK * IR_THRES_DOWN))
#define IR_HEADER_SPACE_MIN ((uint32_t)(IR_HEADER_SPACE * IR_THRES_DOWN))
#define IR_HEADER_MARK_MAX ((uint32_t)(IR_HEADER_MARK * IR_THRES_UP))
#define IR_HEADER_SPACE_MAX ((uint32_t)(IR_HEADER_SPACE * IR_THRES_UP))
#define IR_DIVIDER_SPACE IR_HEADER_MARK
#define IR_DIVIDER_SPACE_MIN IR_HEADER_MARK_MIN
#define IR_DIVIDER_SPACE_MAX IR_HEADER_MARK_MAX
#define IR_SEND_PWM_PIN 0
#define IR_RECEIVER_PIN 2

// =============================================================================
// IR and VL Receiver state enums (used by the wireless .c files)
// =============================================================================

typedef uint8_t VLRecvState;
enum {
  VL_WAITING_HEADER_MARK,
  VL_WAITING_HEADER_SPACE,
  VL_READING_BAUD_MARK,
  VL_READING_BAUD_SPACE,
  VL_READING_DATA_MARK,
  VL_READING_DATA_SPACE,
  VL_READING_DATA_PARITY_MARK,
  VL_READING_DATA_PARITY_SPACE
};

typedef uint8_t IRRecvState;
enum {
  IR_WAITING_HEADER_MARK,
  IR_WAITING_HEADER_SPACE,
  IR_READING_DATA_MARK,
  IR_READING_DATA_SPACE
};

extern BitStream VLReceiver_vlData;
extern VLRecvState VLReceiver_recvState;
extern uint32_t VLReceiver_prevTime;
extern uint8_t VLReceiver_pinState;
extern uint16_t VLReceiver_previousBytes;
extern uint16_t VLReceiver_vlMarkThreshold;
extern uint16_t VLReceiver_vlSpaceThreshold;
extern uint8_t VLReceiver_counter;
extern uint8_t VLReceiver_parityBit;
extern bool VLReceiver_legacy;

uint16_t VLReceiver_bytesReceived(void);

extern BitStream IRReceiver_irData;
extern IRRecvState IRReceiver_recvState;
extern uint32_t IRReceiver_prevTime;
extern uint8_t IRReceiver_pinState;
extern uint32_t IRReceiver_previousBytes;

uint16_t IRReceiver_bytesReceived(void);

// =============================================================================
// ModeLink (internal linked list type for Modes)
// =============================================================================
typedef struct ModeLink ModeLink;
struct ModeLink {
  Mode *m_pInstantiatedMode;
  ByteStream m_storedMode;
  ModeLink *m_next;
  ModeLink *m_prev;
};

ModeLink *ModeLink_next(ModeLink *self);
ModeLink *ModeLink_prev(ModeLink *self);
void ModeLink_init(ModeLink *self, const Mode *src, bool inst);
void ModeLink_initFromStream(ModeLink *self, const ByteStream *src, bool inst);
void ModeLink_cleanup(ModeLink *self);
bool ModeLink_initMode(ModeLink *self, const Mode *mode);
bool ModeLink_appendMode(ModeLink *self, const Mode *next);
bool ModeLink_appendStream(ModeLink *self, const ByteStream *next);
void ModeLink_play(ModeLink *self);
ModeLink *ModeLink_unlinkSelf(ModeLink *self);
void ModeLink_linkAfter(ModeLink *self, ModeLink *link);
void ModeLink_linkBefore(ModeLink *self, ModeLink *link);
Mode *ModeLink_instantiate(ModeLink *self);
void ModeLink_uninstantiate(ModeLink *self);
bool ModeLink_save(ModeLink *self);
ByteStream *ModeLink_buffer(ModeLink *self);
Mode *ModeLink_mode(ModeLink *self);

// =============================================================================
// EditorConnection types
// =============================================================================
typedef uint8_t ReturnCode;
enum {
  RV_FAIL = 0,
  RV_OK,
  RV_WAIT,
};

typedef uint8_t EditorConnectionState;
enum {
  STATE_DISCONNECTED,
  STATE_GREETING,
  STATE_IDLE,
  STATE_PULL_MODES,
  STATE_PULL_MODES_SEND,
  STATE_PULL_MODES_DONE,
  STATE_PUSH_MODES,
  STATE_PUSH_MODES_RECEIVE,
  STATE_PUSH_MODES_DONE,
  STATE_DEMO_MODE,
  STATE_DEMO_MODE_RECEIVE,
  STATE_DEMO_MODE_DONE,
  STATE_CLEAR_DEMO,
  STATE_TRANSMIT_MODE_VL,
  STATE_TRANSMIT_MODE_VL_DONE,
  STATE_PULL_EACH_MODE,
  STATE_PULL_EACH_MODE_COUNT,
  STATE_PULL_EACH_MODE_SEND,
  STATE_PULL_EACH_MODE_WAIT,
  STATE_PULL_EACH_MODE_DONE,
  STATE_PUSH_EACH_MODE,
  STATE_PUSH_EACH_MODE_COUNT,
  STATE_PUSH_EACH_MODE_RECEIVE,
  STATE_PUSH_EACH_MODE_WAIT,
  STATE_PUSH_EACH_MODE_DONE,
  STATE_SET_GLOBAL_BRIGHTNESS,
  STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE,
  STATE_GET_GLOBAL_BRIGHTNESS,
};

// =============================================================================
// ModeShare types
// =============================================================================
typedef uint8_t ModeShareState;
enum {
  SHARE_SEND_RECEIVE,
  SHARE_SEND_RECEIVE_LEGACY,
  SHARE_EXIT,
};

// =============================================================================
// ColorSelect types
// =============================================================================
typedef uint8_t ColorSelectState;
enum {
  COLOR_SELECT_STATE_INIT,
  COLOR_SELECT_STATE_PICK_SLOT,
  COLOR_SELECT_STATE_PICK_HUE1,
  COLOR_SELECT_STATE_PICK_HUE2,
  COLOR_SELECT_STATE_PICK_SAT,
  COLOR_SELECT_STATE_PICK_VAL,
};

// =============================================================================
// Randomizer types
// =============================================================================
typedef uint8_t RandomizeFlags;
enum {
  RANDOMIZE_NONE = 0,
  RANDOMIZE_COLORSET = (1 << 0),
  RANDOMIZE_PATTERN = (1 << 1),
  RANDOMIZE_BOTH = (RANDOMIZE_COLORSET | RANDOMIZE_PATTERN),
};

// =============================================================================
// GlobalBrightness types
// =============================================================================
typedef uint8_t KeychainModeState;
enum {
  KEYCHAIN_MODE_STATE_OFF = 0,
  KEYCHAIN_MODE_STATE_SOLID,
  KEYCHAIN_MODE_STATE_DOPS,
  KEYCHAIN_MODE_STATE_SIGNAL,
  KEYCHAIN_MODE_STATE_COUNT
};

// =============================================================================
// Additional Button functions
// =============================================================================
bool Button_initPin(Button *self, uint8_t pin);
void Button_cleanup(Button *self);

// =============================================================================
// Additional Colorset functions
// =============================================================================
bool Colorset_initPaletteInternal(Colorset *self, uint8_t numColors);

// =============================================================================
// Additional RGBColor functions used in .c files
// =============================================================================
void RGBColor_initFromHSV(RGBColor *self, const HSVColor *hsv);

// =============================================================================
// Menu VTable externs (defined in individual .c files)
// =============================================================================
extern const MenuVTable g_colorSelectMenuVTable;
extern const MenuVTable g_patternSelectMenuVTable;
extern const MenuVTable g_randomizerMenuVTable;
extern const MenuVTable g_globalBrightnessMenuVTable;
extern const MenuVTable g_factoryResetMenuVTable;
extern const MenuVTable g_editorConnectionMenuVTable;
extern const MenuVTable g_modeSharingMenuVTable;

// =============================================================================
// EditorConnection command state
// =============================================================================
typedef struct {
  const char *cmd;
  EditorConnectionState cmdState;
} EditorConnectionCommandState;

// defined as static in EditorConnection.c

// =============================================================================
// Additional Mode functions needed by .c files
// =============================================================================
void Mode_initFromIDArgsSetPtr(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set);
void Mode_initFromEntry(Mode *self, const struct DefaultModeEntry *entry);
void Mode_cleanup(Mode *self);
bool Mode_saveToBuffer(const Mode *self, ByteStream *saveBuffer, uint8_t numLeds);
bool Mode_loadFromBuffer(Mode *self, ByteStream *saveBuffer);
bool Mode_setPatternID(Mode *self, PatternID id);
bool Mode_setPatternMap(Mode *self, LedMap map, PatternID id, const PatternArgs *args, const Colorset *set);
bool Modes_addModeArgsSet(PatternID id, const PatternArgs *args, const Colorset *set);
ModeLink *Modes_getModeLink(uint32_t index);
Mode *Modes_initCurMode(bool force);
bool Modes_saveCurMode(void);

// =============================================================================
// Modes globals
// =============================================================================
extern uint8_t Modes_globalFlags;

// =============================================================================
// Additional Colorset functions
// =============================================================================
void Colorset_initPalette(Colorset *self, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);

// =============================================================================
// Additional VortexEngine functions (embedded-specific)
// =============================================================================
#ifdef VORTEX_EMBEDDED
void VortexEngine_clearOutputPins(void);
void VortexEngine_enableMOSFET(bool enabled);
#endif

#ifdef VORTEX_LIB
bool VortexEngine_isSleeping(void);
#endif

// =============================================================================
// VLSender types (used by VLSender.c)
// =============================================================================
typedef struct VLSenderCallbacks VLSenderCallbacks;

// =============================================================================
// Helpful color macro used in some .c files
// =============================================================================
#ifndef RGB_COLOR
#define RGB_COLOR(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })
#endif
#ifndef COL32
#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })
#endif

#endif // C_TYPES_H
