#ifndef PATTERN_H
#define PATTERN_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "../Leds/LedTypes.h"
#include "../Colors/Colorset.h"
#include "Patterns.h"
#include "PatternArgs.h"

#define MAX_PATTERN_ARGS 8

/*
 *                                Pattern
 *                              /        \
 *                SingleLedPattern        MultiLedPattern
 *                /           \
 *           BasicPattern
 *           /           \
 *      SolidPattern   BlendPattern
 */

#define PATTERN_FLAGS_NONE  0
#define PATTERN_FLAG_MULTI  (1<<0)

#if VORTEX_SLIM == 1
typedef uint8_t arg_offset_t;
#else
typedef uint16_t arg_offset_t;
#endif

#ifdef VORTEX_LIB
#define REGISTER_ARG(self, arg) Pattern_registerArgName((Pattern *)(self), #arg, (arg_offset_t)((uintptr_t)&(arg) - (uintptr_t)(self)))
#else
#define REGISTER_ARG(self, arg) Pattern_registerArg((Pattern *)(self), (arg_offset_t)((uintptr_t)&(arg) - (uintptr_t)(self)))
#endif

typedef struct ByteStream ByteStream;
typedef struct Pattern Pattern;

typedef struct PatternVTable {
  void (*destroy)(Pattern *self);
  void (*play)(Pattern *self);
  void (*init)(Pattern *self);
  void (*bind)(Pattern *self, LedPos pos);
  void (*onBlinkOn)(Pattern *self);
  void (*onBlinkOff)(Pattern *self);
  void (*beginGap)(Pattern *self);
  void (*beginDash)(Pattern *self);
} PatternVTable;

struct Pattern {
  const PatternVTable *vtable;
  PatternID patternID;
  uint8_t patternFlags;
  Colorset colorset;
  LedPos ledPos;
  uint8_t numArgs;
  arg_offset_t argList[MAX_PATTERN_ARGS];
#ifdef VORTEX_LIB
  const char *argNameList[MAX_PATTERN_ARGS];
#endif
};

void Pattern_init(Pattern *self, const PatternArgs *args);
void Pattern_initBase(Pattern *self);
void Pattern_initVirtual(Pattern *self);
void Pattern_destroy(Pattern *self);
void Pattern_bind(Pattern *self, LedPos pos);
void Pattern_bindBase(Pattern *self, LedPos pos);
void Pattern_play(Pattern *self);

#ifdef VORTEX_LIB
void Pattern_skip(Pattern *self, uint32_t ticks);
#endif

bool Pattern_serialize(const Pattern *self, ByteStream *buffer);
bool Pattern_unserialize(Pattern *self, ByteStream *buffer);

void Pattern_setArg(Pattern *self, uint8_t index, uint8_t value);
uint8_t Pattern_getArg(const Pattern *self, uint8_t index);
uint8_t *Pattern_argRef(Pattern *self, uint8_t index);

#ifdef VORTEX_LIB
const char *Pattern_getArgName(const Pattern *self, uint8_t index);
#endif

void Pattern_setArgs(Pattern *self, const PatternArgs *args);
void Pattern_getArgs(const Pattern *self, PatternArgs *args);
uint8_t Pattern_getNumArgs(const Pattern *self);
bool Pattern_equals(const Pattern *self, const Pattern *other);

Colorset Pattern_getColorset(const Pattern *self);
void Pattern_setColorset(Pattern *self, const Colorset *set);
void Pattern_clearColorset(Pattern *self);
void Pattern_setLedPos(Pattern *self, LedPos pos);
PatternID Pattern_getPatternID(const Pattern *self);
LedPos Pattern_getLedPos(const Pattern *self);
uint32_t Pattern_getFlags(const Pattern *self);
bool Pattern_hasFlags(const Pattern *self, uint32_t flags);

#ifdef VORTEX_LIB
void Pattern_registerArgName(Pattern *self, const char *name, arg_offset_t argOffset);
#else
void Pattern_registerArg(Pattern *self, arg_offset_t argOffset);
#endif

#endif
