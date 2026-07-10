#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include "../Menu.h"

typedef enum {
  KEYCHAIN_MODE_STATE_OFF = 0,
  KEYCHAIN_MODE_STATE_SOLID,
  KEYCHAIN_MODE_STATE_DOPS,
  KEYCHAIN_MODE_STATE_SIGNAL,
  KEYCHAIN_MODE_STATE_COUNT
} KeychainModeState;

typedef struct GlobalBrightnessMenu_s {
  Menu base;
  KeychainModeState keychainModeState;
  uint32_t lastStateChange;
  uint8_t colorIndex;
} GlobalBrightnessMenu;

Menu *GlobalBrightnessMenu_Create(RGBColor col, bool advanced);
void GlobalBrightnessMenu_destroy(Menu *self);
bool GlobalBrightnessMenu_init(Menu *self);
MenuAction GlobalBrightnessMenu_run(Menu *self);
void GlobalBrightnessMenu_onShortClick(Menu *self);
void GlobalBrightnessMenu_onLongClick(Menu *self);

extern const MenuVTable g_globalBrightnessMenuVTable;

#endif
