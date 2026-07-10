#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

#include "BasicPattern.h"
#include "../../Time/Timings.h"
#include "../../Colors/ColorTypes.h"

typedef struct BlendPattern {
  BasicPattern base;
  uint8_t blendSpeed;
  uint8_t numFlips;
  RGBColor cur;
  RGBColor next;
  uint8_t flip;
} BlendPattern;

extern const PatternVTable BlendPattern_vtable;

void BlendPattern_init(BlendPattern *self, const PatternArgs *args);
void BlendPattern_initVirtual(Pattern *base);
void BlendPattern_onBlinkOn(Pattern *base);

#endif
