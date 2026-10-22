#ifndef MULTI_LED_PATTERN_H
#define MULTI_LED_PATTERN_H

#include <stdbool.h>
#include "../Pattern.h"

// MultiLedPattern is the base for every pattern that drives the whole led
// map at once rather than a single led. It adds no data of its own, it only
// sets PATTERN_FLAG_MULTI and forces the bound position to LED_COUNT.
typedef struct MultiLedPattern {
  // must remain the first member
  Pattern base;
} MultiLedPattern;

// the vtable a multi-led pattern dispatches through by default
extern const PatternVtable MultiLedPattern_vtable;

// initialize the base portion of a multi-led pattern. The pattern id is
// not assigned here because MultiLedPattern is abstract.
// 'vtable' is the table the derived pattern dispatches through, or
// &MultiLedPattern_vtable to keep the default behaviour.
void MultiLedPattern_init(MultiLedPattern *self, const PatternArgs *args, const PatternVtable *vtable);

// the default vtable implementations
void MultiLedPattern_bind(Pattern *self, LedPos pos);
void MultiLedPattern_doInit(Pattern *self);

#endif
