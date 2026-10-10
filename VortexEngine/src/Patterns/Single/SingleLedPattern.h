#ifndef SINGLE_LED_PATTERN_H
#define SINGLE_LED_PATTERN_H

#include "../Pattern.h"

// SingleLedPattern used to be a thin abstract subclass of Pattern whose only
// behaviour was to call Pattern::init(). In C that distinction disappears,
// so it is simply an alias for the base and existing references keep working.
// The concrete single-led patterns derive from BlinkPattern directly.
typedef Pattern SingleLedPattern;

#endif
