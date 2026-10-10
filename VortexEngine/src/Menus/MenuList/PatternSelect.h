#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include <stdbool.h>
#include "../Menu.h"

#include "../../Colors/Colorset.h"
#include "../../Modes/Mode.h"

typedef struct PatternSelect
{
  Menu m_base;
  // helpful member
  LedPos m_srcLed;
  // the pat select starts by showing the current pattern
  // then the first click begin cycling the list of pats
  bool m_started;
} PatternSelect;

// vtable wiring the derived menu back into the Menu base
extern const MenuVtable PatternSelect_vtable;

PatternSelect *PatternSelect_new(const RGBColor *col, bool advanced);
void PatternSelect_destroy(PatternSelect *self);

bool PatternSelect_init(PatternSelect *self);
MenuAction PatternSelect_run(PatternSelect *self);

// callback after the user selects the target led
void PatternSelect_onLedSelected(PatternSelect *self);

// handlers for clicks
void PatternSelect_onShortClick(PatternSelect *self);
void PatternSelect_onLongClick(PatternSelect *self);

#endif