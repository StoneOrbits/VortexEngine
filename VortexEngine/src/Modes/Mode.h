#ifndef MODE_H
#define MODE_H

#include <inttypes.h>
#include <stdbool.h>
#include "../Leds/LedTypes.h"
#include "../Patterns/Patterns.h"
#include "../VortexConfig.h"
#include "DefaultModes.h"

typedef struct Pattern Pattern;
typedef struct PatternArgs_s PatternArgs;
typedef struct ByteStream ByteStream;
typedef struct Colorset_s Colorset;
typedef struct SingleLedPattern SingleLedPattern;
typedef struct MultiLedPattern MultiLedPattern;

typedef uint8_t ModeFlags;

#define MODE_FLAG_NONE              0
#define MODE_FLAG_MULTI_LED         (1 << 0)
#define MODE_FLAG_SINGLE_LED        (1 << 1)
#define MODE_FLAG_ALL_SAME_SINGLE   (1 << 2)
#define MODE_FLAG_SPARSE_SINGLES    (1 << 3)

typedef struct Mode {
#if VORTEX_SLIM == 0
  Pattern *m_multiPat;
#endif
#if FIXED_LED_COUNT == 0
  uint8_t m_numLeds;
  Pattern **m_singlePats;
#else
  Pattern *m_singlePats[LED_COUNT];
#endif
} Mode;

void Mode_init(Mode *self);
void Mode_initFromEntry(Mode *self, const struct DefaultModeEntry *entry);
void Mode_initFromID(Mode *self, PatternID id, const Colorset *set);
void Mode_initFromIDArgsSet(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set);
void Mode_initFromIDArgsSetPtr(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set);
void Mode_initFromOther(Mode *self, const Mode *other);
void Mode_initCopy(Mode *self, const Mode *other);
void Mode_cleanup(Mode *self);

void Mode_assign(Mode *self, const Mode *other);
bool Mode_equals(const Mode *self, const Mode *other);
bool Mode_notEquals(const Mode *self, const Mode *other);

void Mode_initMode(Mode *self);
void Mode_play(Mode *self);
bool Mode_saveToBuffer(const Mode *self, ByteStream *saveBuffer, uint8_t numLeds);
bool Mode_loadFromBuffer(Mode *self, ByteStream *saveBuffer);
bool Mode_serialize(const Mode *self, ByteStream *buffer, uint8_t numLeds);
bool Mode_unserialize(Mode *self, ByteStream *buffer);
#if FIXED_LED_COUNT == 0
bool Mode_setLedCount(Mode *self, uint8_t numLeds);
#endif
uint8_t Mode_getLedCount(const Mode *self);

const Pattern *Mode_getPatternConst(const Mode *self, LedPos pos);
Pattern *Mode_getPattern(Mode *self, LedPos pos);

Colorset Mode_getColorset(const Mode *self, LedPos pos);
Colorset Mode_getColorsetMut(Mode *self, LedPos pos);

PatternID Mode_getPatternID(const Mode *self, LedPos pos);

bool Mode_setPattern(Mode *self, PatternID pat, LedPos pos, const PatternArgs *args, const Colorset *set);
bool Mode_setPatternMap(Mode *self, LedMap map, PatternID pat, const PatternArgs *args, const Colorset *set);

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

#if MODES_TEST == 1
void Mode_test(void);
#endif

#endif
