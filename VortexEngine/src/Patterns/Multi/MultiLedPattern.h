#ifndef MULTI_LED_PATTERN_H
#define MULTI_LED_PATTERN_H

#include "../Pattern.h"

typedef struct MultiLedPattern {
  Pattern base;
} MultiLedPattern;

extern const PatternVTable MultiLedPattern_vtable;

void MultiLedPattern_init(MultiLedPattern *self, const PatternArgs *args);
void MultiLedPattern_bind(Pattern *self, LedPos pos);
void MultiLedPattern_initVirtual(Pattern *self);

#endif
