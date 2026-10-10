#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <stdbool.h>
#include "../Menu.h"

#include "../../Random/Random.h"
#include "../../Modes/Mode.h"

// this isn't a valid randomization state, if the flags are
// on this state then the user will be prompted to pick
#define RANDOMIZE_NONE    0
// the two main kinds of randomization
#define RANDOMIZE_COLORSET (1 << 0)
#define RANDOMIZE_PATTERN  (1 << 1)
// compound flags both colorset and pattern randomization
#define RANDOMIZE_BOTH     (RANDOMIZE_COLORSET | RANDOMIZE_PATTERN)

typedef struct Randomizer
{
  Menu m_base;
  // random context for each single led
  Random m_singlesRandCtx[LED_COUNT];
#if VORTEX_SLIM == 0
  // random context for the multi led position
  Random m_multiRandCtx;
#endif
  // the time of the last randomization
  uint32_t m_lastRandomization;
  // the randomization flags above
  uint8_t m_flags;
  // auxilliary variable to display cycling hue at menus
  uint8_t m_displayHue;
  // whether still need to select a randomization type
  bool m_needToSelect;
  // whether auto cycling
  bool m_autoCycle;
} Randomizer;

extern const MenuVtable Randomizer_vtable;

Randomizer *Randomizer_new(const RGBColor *col, bool advanced);
void Randomizer_destroy(Randomizer *self);

bool Randomizer_init(Randomizer *self);
MenuAction Randomizer_run(Randomizer *self);

// handlers for clicks
void Randomizer_onShortClick(Randomizer *self);
void Randomizer_onLongClick(Randomizer *self);

// re-roll a new randomization with a given context on an led
bool Randomizer_reRoll(Randomizer *self);

void Randomizer_showRandomizationSelect(Randomizer *self);

#if VORTEX_SLIM == 0
bool Randomizer_reRollMulti(Randomizer *self);
PatternID Randomizer_rollMultiLedPatternID(Randomizer *self, Random *ctx);
#endif
bool Randomizer_reRollSingles(Randomizer *self);
PatternID Randomizer_rollSingleLedPatternID(Randomizer *self, Random *ctx);

// generate a random colorset with a random context
Colorset Randomizer_rollColorset(Randomizer *self, Random *ctx);

// roll a custom pattern by generating random arguments
bool Randomizer_rollCustomPattern(Randomizer *self, Random *ctx, Mode *pMode, LedPos pos);

void Randomizer_traditionalPattern(Random *ctx, PatternArgs *outArgs);
void Randomizer_gapPattern(Random *ctx, PatternArgs *outArgs);
void Randomizer_dashPattern(Random *ctx, PatternArgs *outArgs);
void Randomizer_crushPattern(Random *ctx, PatternArgs *outArgs);

#endif