#ifndef GLOBAL_BRIGHTNESS_H
#define GLOBAL_BRIGHTNESS_H

#include <stdbool.h>
#include "../Menu.h"

// the keychain mode sub states
typedef uint8_t keychain_mode_state;
enum {
  // sleeping / fake off
  KEYCHAIN_MODE_STATE_OFF = 0,
  // solid/tracer
  KEYCHAIN_MODE_STATE_SOLID,
  // dops blink 4 / 16
  KEYCHAIN_MODE_STATE_DOPS,
  // signal blink 16 / 120
  KEYCHAIN_MODE_STATE_SIGNAL,
  // total states
  KEYCHAIN_MODE_STATE_COUNT
};

typedef struct GlobalBrightness
{
  Menu m_base;
  keychain_mode_state m_keychain_modeState;
  uint32_t m_lastStateChange;
  uint8_t m_colorIndex;
} GlobalBrightness;

extern const MenuVtable GlobalBrightness_vtable;

GlobalBrightness *GlobalBrightness_new(const RGBColor *col, bool advanced);
void GlobalBrightness_destroy(GlobalBrightness *self);

bool GlobalBrightness_init(GlobalBrightness *self);
MenuAction GlobalBrightness_run(GlobalBrightness *self);

// handlers for clicks
void GlobalBrightness_onShortClick(GlobalBrightness *self);
void GlobalBrightness_onLongClick(GlobalBrightness *self);

void GlobalBrightness_showBrightnessSelection(GlobalBrightness *self);

void GlobalBrightness_setKeychainModeState(GlobalBrightness *self, keychain_mode_state newState);
MenuAction GlobalBrightness_runKeychainMode(GlobalBrightness *self);

#endif