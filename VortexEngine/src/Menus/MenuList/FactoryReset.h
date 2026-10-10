#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include <stdbool.h>
#include "../Menu.h"

typedef struct FactoryReset
{
  Menu m_base;
} FactoryReset;

extern const MenuVtable FactoryReset_vtable;

FactoryReset *FactoryReset_new(const RGBColor *col, bool advanced);
void FactoryReset_destroy(FactoryReset *self);

bool FactoryReset_init(FactoryReset *self);
MenuAction FactoryReset_run(FactoryReset *self);

// handlers for clicks
void FactoryReset_onShortClick(FactoryReset *self);
void FactoryReset_onLongClick(FactoryReset *self);

void FactoryReset_showReset(FactoryReset *self);

#endif
