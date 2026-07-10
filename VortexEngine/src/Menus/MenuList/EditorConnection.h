#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/Mode.h"

typedef enum {
  STATE_DISCONNECTED,
  STATE_GREETING,
  STATE_IDLE,
  STATE_PULL_MODES,
  STATE_PULL_MODES_SEND,
  STATE_PULL_MODES_DONE,
  STATE_PUSH_MODES,
  STATE_PUSH_MODES_RECEIVE,
  STATE_PUSH_MODES_DONE,
  STATE_DEMO_MODE,
  STATE_DEMO_MODE_RECEIVE,
  STATE_DEMO_MODE_DONE,
  STATE_CLEAR_DEMO,
  STATE_TRANSMIT_MODE_VL,
  STATE_TRANSMIT_MODE_VL_DONE,
  STATE_PULL_EACH_MODE,
  STATE_PULL_EACH_MODE_COUNT,
  STATE_PULL_EACH_MODE_SEND,
  STATE_PULL_EACH_MODE_WAIT,
  STATE_PULL_EACH_MODE_DONE,
  STATE_PUSH_EACH_MODE,
  STATE_PUSH_EACH_MODE_COUNT,
  STATE_PUSH_EACH_MODE_RECEIVE,
  STATE_PUSH_EACH_MODE_WAIT,
  STATE_PUSH_EACH_MODE_DONE,
  STATE_SET_GLOBAL_BRIGHTNESS,
  STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE,
  STATE_GET_GLOBAL_BRIGHTNESS,
} EditorConnectionState;

typedef struct EditorConnectionCommandState_s {
  const char *cmd;
  EditorConnectionState cmdState;
} EditorConnectionCommandState;

typedef enum {
  RV_FAIL = 0,
  RV_OK,
  RV_WAIT,
} ReturnCode;

typedef struct EditorConnectionMenu_s {
  Menu base;
  EditorConnectionState state;
  ByteStream receiveBuffer;
  bool allowReset;
  uint8_t previousModeIndex;
  uint8_t numModesToReceive;
  ReturnCode rv;
} EditorConnectionMenu;

Menu *EditorConnectionMenu_Create(RGBColor col, bool advanced);
void EditorConnectionMenu_destroy(Menu *self);
bool EditorConnectionMenu_init(Menu *self);
MenuAction EditorConnectionMenu_run(Menu *self);
void EditorConnectionMenu_onShortClick(Menu *self);
void EditorConnectionMenu_onLongClick(Menu *self);
void EditorConnectionMenu_leaveMenu(Menu *self, bool doSave);

extern const MenuVTable g_editorConnectionMenuVTable;

#endif
