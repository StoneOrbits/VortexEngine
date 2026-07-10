#ifndef PATTERN_ARGS_H
#define PATTERN_ARGS_H

#include <inttypes.h>
#include <stdbool.h>

#define MAX_ARGS 8

typedef struct ByteStream ByteStream;

typedef uint8_t ArgMap;

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

#define ARGMAP_SET(map, arg)    (map |= ARG(arg))
#define ARGMAP_CLEAR(map, arg)  (map &= ~ARG(arg))
#define ARGMAP_ISSET(map, arg)  ((map & ARG(arg)) != 0)

typedef struct PatternArgs_s {
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
} PatternArgs;

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
bool PatternArgs_notEquals(const PatternArgs *self, const PatternArgs *rhs);
uint8_t PatternArgs_get(const PatternArgs *self, int index);

bool PatternArgs_serialize(const PatternArgs *self, ByteStream *buffer, ArgMap argmap);
ArgMap PatternArgs_unserialize(PatternArgs *self, ByteStream *buffer);

#endif
