#include "Pattern.h"

#include "../Patterns/PatternBuilder.h"
#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"
#include "../VortexConfig.h"

void Pattern_init(Pattern *self, const PatternArgs *args)
{
  (void)args;
  self->vtable = NULL;
  self->patternID = PATTERN_FIRST;
  self->patternFlags = 0;
  Colorset_init(&self->colorset);
  self->ledPos = LED_FIRST;
  self->numArgs = 0;
}

void Pattern_bindBase(Pattern *self, LedPos pos)
{
  self->ledPos = pos;
}

void Pattern_initBase(Pattern *self)
{
  Colorset_resetIndex(&self->colorset);
}

void Pattern_destroy(Pattern *self)
{
  if (self && self->vtable && self->vtable->destroy) {
    self->vtable->destroy(self);
  }
}

void Pattern_bind(Pattern *self, LedPos pos)
{
  if (self && self->vtable && self->vtable->bind) {
    self->vtable->bind(self, pos);
  }
}

void Pattern_initVirtual(Pattern *self)
{
  if (self && self->vtable && self->vtable->init) {
    self->vtable->init(self);
  }
}

void Pattern_play(Pattern *self)
{
  if (self && self->vtable && self->vtable->play) {
    self->vtable->play(self);
  }
}

#ifdef VORTEX_LIB
void Pattern_skip(Pattern *self, uint32_t ticks)
{
  Time_startSimulation();
  for (uint32_t i = 0; i < ticks; ++i) {
    Pattern_play(self);
    Time_tickSimulation();
  }
  Time_endSimulation();
}
#endif

bool Pattern_serialize(const Pattern *self, ByteStream *buffer)
{
  if (!ByteStream_serialize8(buffer, (uint8_t)self->patternID)) {
    return false;
  }
  if (!Colorset_serialize(&self->colorset, buffer)) {
    return false;
  }
  PatternArgs args;
  Pattern_getArgs(self, &args);
  PatternArgs defaults = PatternBuilder_getDefaultArgs(self->patternID);
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

bool Pattern_unserialize(Pattern *self, ByteStream *buffer)
{
  if (!Colorset_unserialize(&self->colorset, buffer)) {
    return false;
  }
  PatternArgs args = PatternBuilder_getDefaultArgs(self->patternID);
  PatternArgs_unserialize(&args, buffer);
  Pattern_setArgs(self, &args);
  return true;
}

void Pattern_setArg(Pattern *self, uint8_t index, uint8_t value)
{
  if (index >= self->numArgs) {
    return;
  }
  *((uint8_t *)self + self->argList[index]) = value;
}

uint8_t Pattern_getArg(const Pattern *self, uint8_t index)
{
  if (index >= self->numArgs) {
    return 0;
  }
  return *((uint8_t *)self + self->argList[index]);
}

uint8_t *Pattern_argRef(Pattern *self, uint8_t index)
{
  if (index >= self->numArgs) {
    index = 0;
  }
  return (uint8_t *)self + self->argList[index];
}

void Pattern_setArgs(Pattern *self, const PatternArgs *args)
{
  for (uint32_t i = 0; i < self->numArgs; ++i) {
    *((uint8_t *)self + self->argList[i]) = args->args[i];
  }
}

void Pattern_getArgs(const Pattern *self, PatternArgs *args)
{
  PatternArgs_init(args);
  for (uint32_t i = 0; i < self->numArgs; ++i) {
    args->args[i] = *((uint8_t *)self + self->argList[i]);
  }
  args->numArgs = self->numArgs;
}

uint8_t Pattern_getNumArgs(const Pattern *self)
{
  return self->numArgs;
}

bool Pattern_equals(const Pattern *self, const Pattern *other)
{
  if (!other) {
    return false;
  }
  if (self->patternID != other->patternID) {
    return false;
  }
  if (!Colorset_equals(&self->colorset, &other->colorset)) {
    return false;
  }
  if (self->numArgs != other->numArgs) {
    return false;
  }
  for (uint8_t i = 0; i < self->numArgs; ++i) {
    if (Pattern_getArg(self, i) != Pattern_getArg(other, i)) {
      return false;
    }
  }
  return true;
}

Colorset Pattern_getColorset(const Pattern *self)
{
  return self->colorset;
}

void Pattern_setColorset(Pattern *self, const Colorset *set)
{
  Colorset_copy(&self->colorset, set);
}

void Pattern_clearColorset(Pattern *self)
{
  Colorset_clear(&self->colorset);
}

void Pattern_setLedPos(Pattern *self, LedPos pos)
{
  self->ledPos = pos;
}

PatternID Pattern_getPatternID(const Pattern *self)
{
  return self->patternID;
}

LedPos Pattern_getLedPos(const Pattern *self)
{
  return self->ledPos;
}

uint32_t Pattern_getFlags(const Pattern *self)
{
  return self->patternFlags;
}

bool Pattern_hasFlags(const Pattern *self, uint32_t flags)
{
  return (self->patternFlags & flags) != 0;
}

#ifdef VORTEX_LIB
void Pattern_registerArgName(Pattern *self, const char *name, arg_offset_t argOffset)
{
  if (self->numArgs >= MAX_PATTERN_ARGS) {
    ERROR_LOG("too many args");
    return;
  }
  self->argNameList[self->numArgs] = name;
  self->argList[self->numArgs++] = argOffset;
}

const char *Pattern_getArgName(const Pattern *self, uint8_t index)
{
  return index >= self->numArgs ? "" : self->argNameList[index];
}
#else
void Pattern_registerArg(Pattern *self, arg_offset_t argOffset)
{
  if (self->numArgs >= MAX_PATTERN_ARGS) {
    ERROR_LOG("too many args");
    return;
  }
  self->argList[self->numArgs++] = argOffset;
}
#endif
