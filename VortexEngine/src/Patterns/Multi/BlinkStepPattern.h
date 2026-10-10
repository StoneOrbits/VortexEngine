#ifndef BLINKSTEP_PATTERN_H
#define BLINKSTEP_PATTERN_H

#include <stdbool.h>
#include "MultiLedPattern.h"

#include "../../Time/Timer.h"
#include "../../Leds/LedTypes.h"

typedef struct BlinkStepPattern BlinkStepPattern;

// BlinkStepVtable adds the four overridable steps on top of the base pattern
// vtable. Concrete patterns supply their own table and reuse the shared
// blink/step play loop, exactly like the old virtual overrides did.
typedef struct BlinkStepVtable {
  // the leading PatternVtable, so this can be used as a PatternVtable*
  PatternVtable base;

  // when the leds blink on (must be provided)
  void (*blinkOn)(BlinkStepPattern *self);
  // when the leds blink off
  void (*blinkOff)(BlinkStepPattern *self);
  // when a step triggers, before blink on
  void (*prestep)(BlinkStepPattern *self);
  // when a step triggers, after blink on
  void (*poststep)(BlinkStepPattern *self);
} BlinkStepVtable;

// BlinkStepPattern is abstract, it cannot be directly instantiated
typedef struct BlinkStepPattern {
  // must remain the first member
  MultiLedPattern base;

  // the step hooks, points at a BlinkStepVtable
  const BlinkStepVtable *bvtable;

  uint8_t m_blinkOnDuration;
  uint8_t m_blinkOffDuration;
  uint8_t m_stepDuration;
  Timer m_blinkTimer;
  Timer m_stepTimer;
} BlinkStepPattern;

// the default hooks, which do nothing useful on their own
extern const BlinkStepVtable BlinkStepPattern_bvtable;

// initialize the base portion of a BlinkStepPattern
void BlinkStepPattern_init(BlinkStepPattern *self, const PatternArgs *args, const BlinkStepVtable *vtable);

// vtable entry points, shared by every BlinkStep derived pattern
void BlinkStepPattern_initImpl(Pattern *self);
void BlinkStepPattern_play(Pattern *self);

// the default hook implementations
void BlinkStepPattern_blinkOn(BlinkStepPattern *self);
void BlinkStepPattern_blinkOff(BlinkStepPattern *self);
void BlinkStepPattern_prestep(BlinkStepPattern *self);
void BlinkStepPattern_poststep(BlinkStepPattern *self);

#endif
