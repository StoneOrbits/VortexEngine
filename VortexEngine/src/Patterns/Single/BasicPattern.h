#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include "SingleLedPattern.h"
#include "../../Time/Timer.h"

#define STATE_DISABLED   0
#define STATE_BLINK_ON   1
#define STATE_ON         2
#define STATE_BLINK_OFF  3
#define STATE_OFF        4
#define STATE_BEGIN_GAP  5
#define STATE_IN_GAP     6
#define STATE_BEGIN_DASH 7
#define STATE_IN_DASH    8
#define STATE_BEGIN_GAP2 9
#define STATE_IN_GAP2    10

typedef struct BasicPattern {
  SingleLedPattern base;
  uint8_t onDuration;
  uint8_t offDuration;
  uint8_t gapDuration;
  uint8_t dashDuration;
  uint8_t groupSize;
  uint8_t groupCounter;
  uint8_t state;
  Timer blinkTimer;
} BasicPattern;

extern const PatternVTable BasicPattern_vtable;

void BasicPattern_init(BasicPattern *self, const PatternArgs *args);
void BasicPattern_play(Pattern *base);
void BasicPattern_initVirtual(Pattern *base);
void BasicPattern_onBlinkOn(Pattern *base);
void BasicPattern_onBlinkOff(Pattern *base);
void BasicPattern_beginGap(Pattern *base);
void BasicPattern_beginDash(Pattern *base);

#endif
