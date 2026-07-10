#ifndef SOLID_PATTERN_H
#define SOLID_PATTERN_H

#include "BasicPattern.h"
#include "../../Time/Timings.h"

typedef struct SolidPattern {
  BasicPattern base;
  uint8_t colIndex;
} SolidPattern;

extern const PatternVTable SolidPattern_vtable;

void SolidPattern_init(SolidPattern *self, const PatternArgs *args);
void SolidPattern_onBlinkOn(Pattern *base);

#endif
