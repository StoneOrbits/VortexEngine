#ifndef BLEND_PATTERN_H
#define BLEND_PATTERN_H

// The blend behaviour was merged into BlinkPattern so that blink and blend
// share a single implementation. BlendPattern is kept as an alias so that
// existing includes of this header keep working.
#include "BasicPattern.h"

typedef BlinkPattern BlendPattern;

#endif
