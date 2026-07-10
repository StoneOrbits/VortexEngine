#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "../Menu.h"

typedef enum {
  SHARE_SEND_RECEIVE,
  SHARE_SEND_RECEIVE_LEGACY,
  SHARE_EXIT,
} ModeShareState;

typedef struct ModeSharingMenu_s {
  Menu base;
  ModeShareState sharingMode;
  uint32_t timeOutStartTime;
  uint32_t lastPercentChange;
  uint8_t lastPercent;
} ModeSharingMenu;

Menu *ModeSharingMenu_Create(RGBColor col, bool advanced);
void ModeSharingMenu_destroy(Menu *self);
bool ModeSharingMenu_init(Menu *self);
MenuAction ModeSharingMenu_run(Menu *self);
void ModeSharingMenu_onLedSelected(Menu *self);
void ModeSharingMenu_onShortClick(Menu *self);
void ModeSharingMenu_onLongClick(Menu *self);

extern const MenuVTable g_modeSharingMenuVTable;

#endif
