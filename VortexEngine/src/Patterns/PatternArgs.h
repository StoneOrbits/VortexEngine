#ifndef PATTERN_ARGS_H
#define PATTERN_ARGS_H

#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>

// maximum number of args, this isn't really configurable so I'm
// not putting it in VortexConfig.h
#define MAX_ARGS 8

typedef struct ByteStream ByteStream;

// a bit map of arguments, this is used to indicate which arguments
// are present in a pattern when saved to storage
typedef uint8_t ArgMap;

// some macros to access each arg in the argmap
#define ARG_NONE  0
#define ARG(x)    (1 << x)
#define ARG1      (1 << 0)
#define ARG2      (1 << 1)
#define ARG3      (1 << 2)
#define ARG4      (1 << 3)
#define ARG5      (1 << 4)
#define ARG6      (1 << 5)
#define ARG7      (1 << 6)
#define ARG8      (1 << 7)
#define ARG_ALL   0xFF

// some helpers to set/clear/check
#define ARGMAP_SET(map, arg)    (map |= ARG(arg))
#define ARGMAP_CLEAR(map, arg)  (map &= ~ARG(arg))
#define ARGMAP_ISSET(map, arg)  ((map & ARG(arg)) != 0)

// a structured method for passing arguments to patterns, there's definitely more dynamic
// ways to approach this but I'm aiming for simple and lightweight here
typedef struct PatternArgs {
  // public access to args
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
    // arg access for basic pattern
    struct {
      uint8_t onDuration;
      uint8_t offDuration;
      uint8_t gapDuration;
      uint8_t dashDuration;
      uint8_t groupSize;
    } basic;
    // arg access for blend
    struct {
      uint8_t onDuration;
      uint8_t offDuration;
      uint8_t gapDuration;
      uint8_t dashDuration;
      uint8_t groupSize;
      uint8_t blendSpeed;
      uint8_t numFlips;
    } blend;
    // arg access for solid
    struct {
      uint8_t onDuration;
      uint8_t offDuration;
      uint8_t gapDuration;
      uint8_t dashDuration;
      uint8_t groupSize;
      uint8_t colorIndex;
    } solid;
  };
  // indicates how many of the above args are filled
  uint8_t numArgs;
} PatternArgs;

// zero-arg and full-width overloads
void PatternArgs_make0(PatternArgs *self);
void PatternArgs_make8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4,
  uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);
void PatternArgs_init0(PatternArgs *self);
void PatternArgs_init8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4,
  uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);

// the arg count of the supplied argument list, used to pick the right
// _makeN / _initN / _addArgsN overload at compile time. the indirection
// through the trailing _(...) macros is required so that PATTERN_ARGS_COUNT
// expands before being pasted with ##.
#define PATTERN_ARGS_NARG(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define PATTERN_ARGS_COUNT(...) \
  PATTERN_ARGS_NARG(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define PATTERN_ARGS_CAT_(a, b) a##b
#define PATTERN_ARGS_MAKE_(N, ...) PATTERN_ARGS_CAT_(PatternArgs_make, N)(__VA_ARGS__)
#define PATTERN_ARGS_INIT_(N, ...) PATTERN_ARGS_CAT_(PatternArgs_init, N)(__VA_ARGS__)
#define PATTERN_ARGS_ADD_(N, ...) PATTERN_ARGS_CAT_(PatternArgs_addArgs, N)(__VA_ARGS__)

// PatternArgs_make(self, ...) / PatternArgs_init(self, ...) for any number of args 1-8.
// self is a named parameter rather than part of __VA_ARGS__ so that it is
// excluded from the argument count.
#define PatternArgs_make(self, ...) \
  PATTERN_ARGS_MAKE_(PATTERN_ARGS_COUNT(__VA_ARGS__), self, __VA_ARGS__)
#define PatternArgs_init(self, ...) \
  PATTERN_ARGS_INIT_(PATTERN_ARGS_COUNT(__VA_ARGS__), self, __VA_ARGS__)

void PatternArgs_make1(PatternArgs *self, uint8_t a1);
void PatternArgs_make2(PatternArgs *self, uint8_t a1, uint8_t a2);
void PatternArgs_make3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3);
void PatternArgs_make4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
void PatternArgs_make5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
void PatternArgs_make6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
void PatternArgs_make7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);

void PatternArgs_init1(PatternArgs *self, uint8_t a1);
void PatternArgs_init2(PatternArgs *self, uint8_t a1, uint8_t a2);
void PatternArgs_init3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3);
void PatternArgs_init4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
void PatternArgs_init5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
void PatternArgs_init6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
void PatternArgs_init7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);

// append arguments one at a time
void PatternArgs_addArgs1(PatternArgs *self, uint8_t a1);
void PatternArgs_addArgs2(PatternArgs *self, uint8_t a1, uint8_t a2);
void PatternArgs_addArgs3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3);
void PatternArgs_addArgs4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
void PatternArgs_addArgs5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
void PatternArgs_addArgs6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
void PatternArgs_addArgs7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);
void PatternArgs_addArgs8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);

#define PatternArgs_addArgsN(self, ...) \
  PATTERN_ARGS_ADD_(PATTERN_ARGS_COUNT(__VA_ARGS__), self, __VA_ARGS__)

// comparison operators
bool PatternArgs_notEqual(const PatternArgs *self, const PatternArgs *rhs);
bool PatternArgs_equal(const PatternArgs *self, const PatternArgs *rhs);

// index operator
uint8_t PatternArgs_at(const PatternArgs *self, int index);

// serialize the pattern args with a specific mapping of which args to store
bool PatternArgs_serialize(const PatternArgs *self, ByteStream *buffer, ArgMap argmap);
bool PatternArgs_serializeAll(const PatternArgs *self, ByteStream *buffer);
// unserialize the pattern args and return the argmap of which args were loaded
// NOTE: You should start with an instance of the default args before you unserialize
ArgMap PatternArgs_unserialize(PatternArgs *self, ByteStream *buffer);

#endif
