#pragma once

#include <inttypes.h>

#include "../VortexConfig.h"

// These error codes follow a pattern when blinked
// The hundreds column is the number of r blinks
// The tens column is the number of g blinks
// The ones column is the number of b blinks
// There is a gray space after each round of blinks
#define ERROR_NONE    0

  // For example this would just be 'B' (1 blink)
#define ERROR_EXAMPLE1    1
  // this would be "R R G G G B"
#define ERROR_EXAMPLE2    231

#if VORTEX_ERROR_BLINK == 0

// normal builds don't do anything
#define FATAL_ERROR(err)

#else

// set an error code and blink it
#define FATAL_ERROR(err) setError(err);

// set an error code
void setError(uint16_t err);
// fetch the current error code
uint16_t getError();
// blink the current error
void blinkError();

#endif
