#include <string.h>
#include "Pattern.h"

#include "../Patterns/PatternBuilder.h"
#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include "../VortexConfig.h"

const PatternVtable Pattern_baseVtable = {
  .destroy = Pattern_destroy,
  .bind = Pattern_baseBind,
  .init = Pattern_baseInit,
  .play = Pattern_basePlay
};

void Pattern_initBase(Pattern *self, PatternID id, const PatternVtable *vtable)
{
  self->vtable = vtable;
  self->m_patternID = id;
  self->m_patternFlags = 0;
  Colorset_make(&self->m_colorset);
  self->m_ledPos = LED_FIRST;
  self->m_numArgs = 0;
  memset((void *)self->m_argList, 0, sizeof(self->m_argList));
#ifdef VORTEX_LIB
  memset((void *)self->m_argNameList, 0, sizeof(self->m_argNameList));
#endif
}

void Pattern_destroy(Pattern *self)
{
  if (!self) {
    return;
  }
  // run the derived destructor hook; this does *not* free the object,
  // the caller is responsible for vfree()ing it after this returns
  if (self->vtable && self->vtable->destroy && self->vtable->destroy != Pattern_destroy) {
    self->vtable->destroy(self);
  }
}

void Pattern_bind(Pattern *self, LedPos pos)
{
  self->vtable->bind(self, pos);
}

void Pattern_init(Pattern *self)
{
  self->vtable->init(self);
}

void Pattern_play(Pattern *self)
{
  self->vtable->play(self);
}

void Pattern_baseBind(Pattern *self, LedPos pos)
{
  self->m_ledPos = pos;
}

void Pattern_baseInit(Pattern *self)
{
  Colorset_resetIndex(&self->m_colorset);
}

void Pattern_basePlay(Pattern *self)
{
}

#ifdef VORTEX_LIB
void Pattern_skip(Pattern *self, uint32_t ticks)
{
  Time_startSimulation();
  for (uint32_t i = 0; i < ticks; ++i) {
    Pattern_play(self);  // simulate playing the pattern
    Time_tickSimulation();
  }
  Time_endSimulation();
}
#endif

// must override the serialize routine to save the pattern
bool Pattern_serialize(const Pattern *self, ByteStream *buffer)
{
  if (!ByteStream_serialize8(buffer, (uint8_t)self->m_patternID)) {
    return false;
  }
  if (!Colorset_serialize(&self->m_colorset, buffer)) {
    return false;
  }
  PatternArgs args;
  Pattern_getArgs(self, &args);
  PatternArgs defaults;
  PatternBuilder_getDefaultArgs(self->m_patternID, &defaults);
  // generate a bitmap of which args are defaulted
  uint8_t argmap = ARG_NONE;
  for (uint32_t i = 0; i < MAX_ARGS; ++i) {
    if (args.args[i] != defaults.args[i]) {
      ARGMAP_SET(argmap, i);
    }
  }
  if (!PatternArgs_serialize(&args, buffer, argmap)) {
    return false;
  }
  return true;
}

// must override unserialize to load patterns
bool Pattern_unserialize(Pattern *self, ByteStream *buffer)
{
  // don't unserialize the pattern ID because it is already
  // unserialized by the pattern builder to decide which pattern
  // to instantiate, instead only unserialize the colorset
  if (!Colorset_unserialize(&self->m_colorset, buffer)) {
    return false;
  }
  // start with the default args for this pattern
  PatternArgs args;
  PatternBuilder_getDefaultArgs(self->m_patternID, &args);
  // then unserialize any different args overtop of the defaults
  PatternArgs_unserialize(&args, buffer);
  Pattern_setArgs(self, &args);
  return true;
}

void Pattern_setArg(Pattern *self, uint8_t index, uint8_t value)
{
  if (index >= self->m_numArgs) {
    return;
  }
  *((uint8_t *)self + self->m_argList[index]) = value;
}

uint8_t Pattern_getArg(const Pattern *self, uint8_t index)
{
  if (index >= self->m_numArgs) {
    return 0;
  }
  return *((uint8_t *)self + self->m_argList[index]);
}

uint8_t *Pattern_argRef(Pattern *self, uint8_t index)
{
  if (index >= self->m_numArgs) {
    index = 0;
  }
  return ((uint8_t *)self + self->m_argList[index]);
}

void Pattern_setArgs(Pattern *self, const PatternArgs *args)
{
  for (uint32_t i = 0; i < self->m_numArgs; ++i) {
    *((uint8_t *)self + self->m_argList[i]) = args->args[i];
  }
}

void Pattern_getArgs(const Pattern *self, PatternArgs *args)
{
  PatternArgs_init0(args);
  for (uint32_t i = 0; i < self->m_numArgs; ++i) {
    PatternArgs_addArgs1(args, *((uint8_t *)self + self->m_argList[i]));
  }
}

bool Pattern_equals(Pattern *self, const Pattern *other)
{
  if (!other) {
    return false;
  }
  // compare pattern id
  if (self->m_patternID != other->m_patternID) {
    return false;
  }
  // then colorset
  if (!Colorset_equalsPtr(&self->m_colorset, &other->m_colorset)) {
    return false;
  }
  // number of args
  if (self->m_numArgs != other->m_numArgs) {
    return false;
  }
  // compare each arg
  for (uint8_t i = 0; i < self->m_numArgs; ++i) {
    if (Pattern_getArg(self, i) != Pattern_getArg(other, i)) {
      return false;
    }
  }
  return true;
}

// change the colorset
void Pattern_setColorset(Pattern *self, const Colorset *set)
{
  Colorset_set(&self->m_colorset, set);
}

void Pattern_clearColorset(Pattern *self)
{
  Colorset_clear(&self->m_colorset);
}

#ifdef VORTEX_LIB
void Pattern_registerArg(Pattern *self, const char *name, arg_offset_t argOffset)
#else
void Pattern_registerArg(Pattern *self, arg_offset_t argOffset)
#endif
{
  if (self->m_numArgs >= MAX_PATTERN_ARGS) {
    ERROR_LOG("too many args");
    return;
  }
#ifdef VORTEX_LIB
  self->m_argNameList[self->m_numArgs] = name;
#endif
  self->m_argList[self->m_numArgs++] = argOffset;
}
