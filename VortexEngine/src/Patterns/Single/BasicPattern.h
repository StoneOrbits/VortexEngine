#ifndef BASIC_PATTERN_H
#define BASIC_PATTERN_H

#include <stdbool.h>
#include "../Pattern.h"
#include "../../Time/Timer.h"

// the various different blinking states the pattern can be in
typedef uint8_t BlinkPatternState;
enum {
  // the led is disabled (there is no on or dash)
  STATE_DISABLED,

  // the pattern is blinking on the next color in the set
  STATE_BLINK_ON,
  STATE_ON,

  // the pattern is blinking off
  STATE_BLINK_OFF,
  STATE_OFF,

  // the pattern is starting a gap after a colorset
  STATE_BEGIN_GAP,
  STATE_IN_GAP,

  // the pattern is beginning a dash after a colorset or gap
  STATE_BEGIN_DASH,
  STATE_IN_DASH,

  // the pattern is starting a gap after a dash
  STATE_BEGIN_GAP2,
  STATE_IN_GAP2,
};

// forward declare so the vtable can name the type
typedef struct BlinkPattern BlinkPattern;

// BlinkPatternVtable extends the base pattern vtable with the four blinking
// hooks. Blend and solid behaviour is selected by pointing these at a
// different implementation, which is how the former BlendPattern subclass
// and the SolidPattern subclass keep working without their own structs.
typedef struct BlinkPatternVtable {
  // the leading PatternVtable, so a BlinkPatternVtable* can be used
  // wherever a PatternVtable* is expected
  PatternVtable base;

  // when blinking on
  void (*onBlinkOn)(BlinkPattern *self);
  // when blinking off
  void (*onBlinkOff)(BlinkPattern *self);
  // after a colorset
  void (*beginGap)(BlinkPattern *self);
  // after a colorset or gap
  void (*beginDash)(BlinkPattern *self);
} BlinkPatternVtable;

// BlinkPattern is the single implementation that covers every blink, dash,
// gap and blend pattern. The blink/dash behaviour and the colour blending
// behaviour live side by side in one struct; m_isBlend selects which of the
// two is active for a given pattern id. The blend arguments are only
// registered for blend pattern ids so that the argument count reported to
// the editor and to the savefile stays identical to before the merge.
struct BlinkPattern {
  // must remain the first member, the base vtable dispatch relies on it
  Pattern base;

  // the blinking hooks, points at a BlinkPatternVtable
  const BlinkPatternVtable *bvtable;

  // the parameters of the pattern
  uint8_t m_onDuration;
  uint8_t m_offDuration;
  uint8_t m_gapDuration;
  uint8_t m_dashDuration;
  uint8_t m_groupSize;

  // blend arguments, only registered for the blend pattern ids
  uint8_t m_blendSpeed;
  uint8_t m_numFlips;

  uint8_t m_groupCounter;

  // whether this instance runs the colour blending behaviour
  bool m_isBlend;

  // the state of the current pattern
  BlinkPatternState m_state;

  // the blink timer used to measure blink timings
  Timer m_blinkTimer;

  // current color and target blend color
  RGBColor m_cur;
  RGBColor m_next;

  // the current flip counter
  uint8_t m_flip;
};

// the historical name is kept as an alias so existing includes and
// signatures referring to BasicPattern continue to resolve
typedef BlinkPattern BasicPattern;

// the vtable every BlinkPattern dispatches through
extern const BlinkPatternVtable BlinkPattern_bvtable;
// the plain PatternVtable view of the above, for the base dispatch
#define BlinkPattern_vtable (BlinkPattern_bvtable.base)

// initialize a BlinkPattern for the given pattern id, the id decides
// whether the blend arguments are registered and which behaviour runs
void BlinkPattern_init(BlinkPattern *self, PatternID id, const PatternArgs *args);
void BlinkPattern_initWithVtable(BlinkPattern *self, PatternID id, const PatternArgs *args, const BlinkPatternVtable *vtable);

// allocate and initialize a BlinkPattern for the given pattern id
BlinkPattern *BlinkPattern_make(PatternID id, const PatternArgs *args);

// vtable entry points
void BlinkPattern_destroy(Pattern *self);
void BlinkPattern_bind(Pattern *self, LedPos pos);
void BlinkPattern_doInit(Pattern *self);
void BlinkPattern_play(Pattern *self);

// the blinking hooks, these are dispatched through the vtable so that
// subclasses can replace them without needing their own play loop
void BlinkPattern_onBlinkOn(BlinkPattern *self);
void BlinkPattern_onBlinkOff(BlinkPattern *self);
void BlinkPattern_beginGap(BlinkPattern *self);
void BlinkPattern_beginDash(BlinkPattern *self);

// whether the given pattern id uses the colour blending behaviour
bool BlinkPattern_isBlendID(PatternID id);

#endif
