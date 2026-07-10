#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "../Menu.h"

#include "../../Random/Random.h"
#include "../../Modes/Mode.h"

typedef struct Mode Mode;

typedef enum {
  RANDOMIZE_NONE = 0,
  RANDOMIZE_COLORSET = (1 << 0),
  RANDOMIZE_PATTERN = (1 << 1),
  RANDOMIZE_BOTH = (RANDOMIZE_COLORSET | RANDOMIZE_PATTERN)
} RandomizeFlags;

typedef struct RandomizerMenu_s {
  Menu base;
  Random singlesRandCtx[LED_COUNT];
#if VORTEX_SLIM == 0
  Random multiRandCtx;
#endif
  uint32_t lastRandomization;
  uint8_t flags;
  uint8_t displayHue;
  bool needToSelect;
  bool autoCycle;
} RandomizerMenu;

Menu *RandomizerMenu_Create(RGBColor col, bool advanced);
void RandomizerMenu_destroy(Menu *self);
bool RandomizerMenu_init(Menu *self);
MenuAction RandomizerMenu_run(Menu *self);
void RandomizerMenu_onShortClick(Menu *self);
void RandomizerMenu_onLongClick(Menu *self);

extern const MenuVTable g_randomizerMenuVTable;

#endif
