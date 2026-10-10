#ifndef MODE_H
#define MODE_H

#include <stdbool.h>
#include "../Leds/LedTypes.h"
#include "../Patterns/Patterns.h"
#include "../VortexConfig.h"
#include "DefaultModes.h"

#include "../Patterns/Pattern.h"
#include "../Colors/Colorset.h"

typedef uint8_t ModeFlags;

// Bitflags for saving modes
#define MODE_FLAG_NONE              0
// the mode is utilizing a multi-led pattern
#define MODE_FLAG_MULTI_LED         (1 << 0)
// the mode is utilizing at least one single-led pattern
#define MODE_FLAG_SINGLE_LED        (1 << 1)
// the mode is utilizing the same single-led pattern on each finger
// optimization will be done when saving and only one led is saved
#define MODE_FLAG_ALL_SAME_SINGLE   (1 << 2)
// the mode is missing leds on some single slots, an led map is
// present in savefile in order to convey which leds are missing
#define MODE_FLAG_SPARSE_SINGLES    (1 << 3)

// A mode is the container for instances of patterns. A pattern
// must be bound to a colorset and led position with Pattern_bind()
// and the mode allows that to be done in one step. Other things
// can be done in the mode such as accelerometer reaction,
// microphone reaction, timed colorset or pattern switches, or any
// other kind of logic that would involve manipulating the pattern
// or colorset as a whole
typedef struct Mode
{
#if VORTEX_SLIM == 0
  // the multi-led pattern slot is only present in non-slim builds
  Pattern *m_multiPat;
#endif
#if FIXED_LED_COUNT == 0
  // the number of leds the mode is targetting
  uint8_t m_numLeds;
  // list of pointers to Patterns, one for each led
  Pattern **m_singlePats;
#else
  Pattern *m_singlePats[LED_COUNT];
#endif
} Mode;

// initialize the mode to its initial state
void Mode_init(Mode *self);
// initialize all the patterns held by the mode (play/led setup)
void Mode_initPatterns(Mode *self);
#if FIXED_LED_COUNT == 0
// initialize the mode with a specific led count
void Mode_initLeds(Mode *self, uint8_t numLeds);
#endif
// initialize the mode from a default mode table entry
void Mode_initDefault(Mode *self, const DefaultModeEntry *entry);
// initialize the mode with a single pattern across all leds
void Mode_initPatternSet(Mode *self, PatternID id, const Colorset *set);
void Mode_initPatternArgsSet(Mode *self, PatternID id, const PatternArgs *args, const Colorset *set);
// deep copy another mode into this one (initializes self first)
void Mode_copy(Mode *self, const Mode *other);
// release all patterns held by the mode
void Mode_destroy(Mode *self);

// copy the contents of another mode over this one (assignment operator)
void Mode_assign(Mode *self, const Mode *other);

// equality operators
bool Mode_opEquals(const Mode *self, const Mode *other);
bool Mode_opNotEquals(const Mode *self, const Mode *other);

// Play the mode
void Mode_play(Mode *self);

// save/load from a buffer for individual mode sharing
bool Mode_saveToBuffer(const Mode *self, ByteStream *saveBuffer, uint8_t numLeds);
bool Mode_loadFromBuffer(Mode *self, ByteStream *saveBuffer);

// save the mode to serial
bool Mode_serialize(const Mode *self, ByteStream *buffer, uint8_t numLeds);
// load the mode from serial (optional led count)
bool Mode_unserialize(Mode *self, ByteStream *buffer);

// mode comparison
bool Mode_equals(const Mode *self, const Mode *other);

#if FIXED_LED_COUNT == 0
// change the internal pattern count in the mode object
bool Mode_setLedCount(Mode *self, uint8_t numLeds);
#endif
uint8_t Mode_getLedCount(const Mode *self);

// get the pattern at a position
const Pattern *Mode_getPattern(const Mode *self, LedPos pos);
Pattern *Mode_getPatternMutable(Mode *self, LedPos pos);

// get the colorset at a position
Colorset Mode_getColorset(const Mode *self, LedPos pos);

// get the pattern ID of the given pattern
PatternID Mode_getPatternID(const Mode *self, LedPos pos);

// change the pattern on a mode (NOTE: you may need to call Mode_init() after!)
bool Mode_setPattern(Mode *self, PatternID pat, LedPos pos, const PatternArgs *args, const Colorset *set);
bool Mode_setPatternMap(Mode *self, LedMap map, PatternID pat, const PatternArgs *args, const Colorset *set);

// copy a pattern from another mode ledpos into this mode
void Mode_copyPatternFrom(Mode *self, const Mode *other, LedPos to, LedPos from);

// swap two patterns
void Mode_swapPatterns(Mode *self, LedPos a, LedPos b);

// set colorset at a specific position
bool Mode_setColorset(Mode *self, const Colorset *set, LedPos pos);
// set colorset at each position in a map
bool Mode_setColorsetMap(Mode *self, LedMap map, const Colorset *set);

// clear stored patterns in various ways
void Mode_clearPattern(Mode *self, LedPos pos);
void Mode_clearPatternMap(Mode *self, LedMap map);

// clear colorset in various ways
void Mode_clearColorset(Mode *self, LedPos pos);
void Mode_clearColorsetMap(Mode *self, LedMap map);

// set/get a single argument on various positions
void Mode_setArg(Mode *self, uint8_t param, uint8_t value, LedMap map);
uint8_t Mode_getArg(Mode *self, uint8_t index, LedPos pos);

// get the flags associated with this mode
ModeFlags Mode_getFlags(const Mode *self);

// whether a multi-led pattern is present in the mode
bool Mode_hasMultiLed(const Mode *self);
// whether at least one single-led pattern is present in the mode
bool Mode_hasSingleLed(const Mode *self);
// whether this mode has all same single-led patterns
bool Mode_hasSameSingleLed(const Mode *self);
// whether this mode has sparse single-led patterns (missing some slots)
bool Mode_hasSparseSingleLed(const Mode *self);

// whether the mode contains no patterns, or all patterns are PATTERN_NONE
bool Mode_isEmpty(const Mode *self);

// get the led map of which singles are set
LedMap Mode_getSingleLedMap(const Mode *self);

// whether this mode purely a multi-led pattern with no singles
bool Mode_isMultiLed(const Mode *self);

#if MODES_TEST == 1
void Mode_test(Mode *self);
#endif

#endif
