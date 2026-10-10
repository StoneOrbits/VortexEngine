#ifndef PATTERN_H
#define PATTERN_H

#include <stdbool.h>
#include "../Leds/LedTypes.h"
#include "../Colors/Colorset.h"

#include "Patterns.h"
#include "PatternArgs.h"

#include <stddef.h>

#define MAX_PATTERN_ARGS 8

// The heirarchy of pattern currently looks like this:
/*
 *                                pattern*
 *                              /        \
 *                single led pat*           multi led pat*
 *                /           \                /
 *           basic pat       tracer pat       hybrid pat*
 *           /                               /           \
 *      Advanced pattern                 Rabbit Pattern   sequenced pattern
 *
 *     * = abstract class that cannot be instantiated
 */

#define PATTERN_FLAGS_NONE  0

// the pattern is a multi-pattern
#define PATTERN_FLAG_MULTI  (1<<0)

// and arg_offset_t is the distance from the base of the object
// to the desired argument, in slim vortex we only need a single byte
// but in multi-led patterns they can be a bit further in
#if VORTEX_SLIM == 1
typedef uint8_t arg_offset_t;
#else
typedef uint16_t arg_offset_t;
#endif

// macro to register args of a pattern. C has no 'this', so the containing
// type has to be named explicitly to compute the offset of the member.
#ifdef VORTEX_LIB
#define REGISTER_ARG(self, type, arg) \
  Pattern_registerArg(self, #arg, (arg_offset_t)offsetof(type, arg));
#else
#define REGISTER_ARG(self, type, arg) \
  Pattern_registerArg(self, (arg_offset_t)offsetof(type, arg));
#endif

typedef struct ByteStream ByteStream;
typedef struct Pattern Pattern;

// the virtual method table for a pattern. 'self' is always the concrete
// pattern pointer, so embedded base structs are reachable by casting down.
typedef struct PatternVtable {
  // destructor, so derived patterns can release their own allocations
  void (*destroy)(Pattern *self);
  // bind a colorset and position to the pattern
  void (*bind)(Pattern *self, LedPos pos);
  // init the pattern to initial state
  void (*init)(Pattern *self);
  // pure virtual in the original hierarchy, every concrete pattern must
  // supply an implementation here
  void (*play)(Pattern *self);
} PatternVtable;

struct Pattern
{
  // the vtable this pattern dispatches through
  const PatternVtable *vtable;

  // the ID of this pattern (set by pattern builder)
  PatternID m_patternID;
  // any flags the pattern has
  uint8_t m_patternFlags;
  // a copy of the colorset that this pattern is initialized with
  Colorset m_colorset;
  // the Led the pattern is running on
  LedPos m_ledPos;

  uint8_t m_numArgs;
  arg_offset_t m_argList[MAX_PATTERN_ARGS];

#ifdef VORTEX_LIB
  const char *m_argNameList[MAX_PATTERN_ARGS];
#endif
};

// the base vtable, provides the default behaviour of the non-pure virtuals
extern const PatternVtable Pattern_baseVtable;

// initialize the base portion of a pattern and select the vtable to
// dispatch through. derived initializers call this first, then populate
// their own members and register their args.
void Pattern_initBase(Pattern *self, PatternID id, const PatternVtable *vtable);

// virtual entry points, these dispatch through the vtable
void Pattern_destroy(Pattern *self);
void Pattern_bind(Pattern *self, LedPos pos);
void Pattern_init(Pattern *self);
void Pattern_play(Pattern *self);

// the base implementations, used as the default vtable entries and
// called directly by derived patterns in place of Pattern::bind() etc
void Pattern_baseBind(Pattern *self, LedPos pos);
void Pattern_baseInit(Pattern *self);
void Pattern_basePlay(Pattern *self);

#ifdef VORTEX_LIB
// skip the pattern ahead some ticks
void Pattern_skip(Pattern *self, uint32_t ticks);
#endif

// serialize and unserialize a pattern to a bytestream
bool Pattern_serialize(const Pattern *self, ByteStream *buffer);
bool Pattern_unserialize(Pattern *self, ByteStream *buffer);

// get or set a single arg
void Pattern_setArg(Pattern *self, uint8_t index, uint8_t value);
uint8_t Pattern_getArg(const Pattern *self, uint8_t index);

uint8_t *Pattern_argRef(Pattern *self, uint8_t index);

#ifdef VORTEX_LIB
// get the name of an arg
static inline const char *Pattern_getArgName(const Pattern *self, uint8_t index)
{
  return index >= self->m_numArgs ? "" : self->m_argNameList[index];
}
#endif

// get or set the entire list of pattern args
void Pattern_setArgs(Pattern *self, const PatternArgs *args);
void Pattern_getArgs(const Pattern *self, PatternArgs *args);

// number of args the pattern has
static inline uint8_t Pattern_getNumArgs(const Pattern *self)
{
  return self->m_numArgs;
}

// comparison to other pattern
// NOTE: That may cause problems because the parameter is still a Pattern *
//       which means comparison would need to cast the other upwards first
// NOTE2: Removing virtual because this probably shouldn't be overridden
bool Pattern_equals(Pattern *self, const Pattern *other);

// change the colorset
static inline Colorset Pattern_getColorset(const Pattern *self)
{
  return self->m_colorset;
}

void Pattern_setColorset(Pattern *self, const Colorset *set);
void Pattern_clearColorset(Pattern *self);

// change the led position
static inline void Pattern_setLedPos(Pattern *self, LedPos pos)
{
  self->m_ledPos = pos;
}

// get/set the ID of the pattern (set by mode builder)
static inline PatternID Pattern_getPatternID(const Pattern *self)
{
  return self->m_patternID;
}

static inline void Pattern_setPatternID(Pattern *self, PatternID id)
{
  self->m_patternID = id;
}

// get a pointer to the colorset that is bound to the pattern
static inline LedPos Pattern_getLedPos(const Pattern *self)
{
  return self->m_ledPos;
}

// get the pattern flags
static inline uint32_t Pattern_getFlags(const Pattern *self)
{
  return self->m_patternFlags;
}

static inline void Pattern_setFlags(Pattern *self, uint32_t flags)
{
  self->m_patternFlags = (uint8_t)flags;
}

static inline bool Pattern_hasFlags(const Pattern *self, uint32_t flags)
{
  return (self->m_patternFlags & flags) != 0;
}

// register an arg, called via the REGISTER_ARG macro
#ifdef VORTEX_LIB
void Pattern_registerArg(Pattern *self, const char *name, arg_offset_t argOffset);
#else
void Pattern_registerArg(Pattern *self, arg_offset_t argOffset);
#endif

#endif
