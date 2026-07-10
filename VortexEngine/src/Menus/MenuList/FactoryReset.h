#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include "../Menu.h"

typedef struct FactoryResetMenu_s {
  Menu base;
} FactoryResetMenu;

Menu *FactoryResetMenu_Create(RGBColor col, bool advanced);
void FactoryResetMenu_destroy(Menu *self);
bool FactoryResetMenu_init(Menu *self);
MenuAction FactoryResetMenu_run(Menu *self);
void FactoryResetMenu_onShortClick(Menu *self);
void FactoryResetMenu_onLongClick(Menu *self);

extern const MenuVTable g_factoryResetMenuVTable;

#endif
