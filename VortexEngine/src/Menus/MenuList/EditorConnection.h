#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include <stdbool.h>
#include "../Menu.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/Mode.h"

// various return codes that internal functions use
typedef uint8_t ReturnCode;
enum {
  RV_FAIL = 0,
  RV_OK,
  RV_WAIT,
};

typedef uint8_t EditorConnectionState;
enum {
  // the editor is not connected
  STATE_DISCONNECTED,

  // Sending the greeting message
  STATE_GREETING,

  // entirely idle, waiting for commands
  STATE_IDLE,

  // editor pulls the modes from device, then wait for the sent modes ack
  STATE_PULL_MODES,
  STATE_PULL_MODES_SEND,
  STATE_PULL_MODES_DONE,

  // editor pushes modes to device, then waits for done
  STATE_PUSH_MODES,
  STATE_PUSH_MODES_RECEIVE,
  STATE_PUSH_MODES_DONE,

  // editor pushes mode to device for demo while idle
  STATE_DEMO_MODE,
  STATE_DEMO_MODE_RECEIVE,
  STATE_DEMO_MODE_DONE,

  // editor tells device to clear the demo preview, device acknowledge
  STATE_CLEAR_DEMO,

  // transmit the mode over visible light
  STATE_TRANSMIT_MODE_VL,
  STATE_TRANSMIT_MODE_VL_DONE,

  // editor pulls the modes from device (safer version)
  STATE_PULL_EACH_MODE,
  STATE_PULL_EACH_MODE_COUNT,
  STATE_PULL_EACH_MODE_SEND,
  STATE_PULL_EACH_MODE_WAIT,
  STATE_PULL_EACH_MODE_DONE,

  // editor pushes modes to device (safer version)
  STATE_PUSH_EACH_MODE,
  STATE_PUSH_EACH_MODE_COUNT,
  STATE_PUSH_EACH_MODE_RECEIVE,
  STATE_PUSH_EACH_MODE_WAIT,
  STATE_PUSH_EACH_MODE_DONE,

  // set global brightness
  STATE_SET_GLOBAL_BRIGHTNESS,
  STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE,

  // get global brightness
  STATE_GET_GLOBAL_BRIGHTNESS,
};

typedef struct EditorConnection
{
  Menu m_base;
  // state of the editor
  EditorConnectionState m_state;
  // the data that is received
  ByteStream m_receiveBuffer;
  // Whether at least one command has been received yet
  bool m_allowReset;
  // the mode index to return to after iterating the modes to send them
  uint8_t m_previousModeIndex;
  // the number of modes that should be received
  uint8_t m_numModesToReceive;
  // internal return value tracker
  ReturnCode m_rv;
} EditorConnection;

extern const MenuVtable EditorConnection_vtable;

EditorConnection *EditorConnection_new(const RGBColor *col, bool advanced);
void EditorConnection_destroy(EditorConnection *self);

bool EditorConnection_init(EditorConnection *self);
MenuAction EditorConnection_run(EditorConnection *self);

// handlers for clicks
void EditorConnection_onShortClick(EditorConnection *self);
void EditorConnection_onLongClick(EditorConnection *self);

void EditorConnection_leaveMenu(EditorConnection *self, bool doSave);

void EditorConnection_clearDemo(EditorConnection *self);
void EditorConnection_handleErrors(EditorConnection *self);
void EditorConnection_handleCommand(EditorConnection *self);
void EditorConnection_handleState(EditorConnection *self);
void EditorConnection_showEditor(EditorConnection *self);
void EditorConnection_receiveData(EditorConnection *self);
void EditorConnection_sendModes(EditorConnection *self);
void EditorConnection_sendModeCount(EditorConnection *self);
void EditorConnection_sendCurMode(EditorConnection *self);
void EditorConnection_sendCurModeVL(EditorConnection *self);
ReturnCode EditorConnection_sendBrightness(EditorConnection *self);
ReturnCode EditorConnection_receiveBuffer(EditorConnection *self, ByteStream *buffer);
ReturnCode EditorConnection_receiveModes(EditorConnection *self);
ReturnCode EditorConnection_receiveModeCount(EditorConnection *self);
ReturnCode EditorConnection_receiveMode(EditorConnection *self);
ReturnCode EditorConnection_receiveDemoMode(EditorConnection *self);
ReturnCode EditorConnection_receiveMessage(EditorConnection *self, const char *message);
ReturnCode EditorConnection_receiveBrightness(EditorConnection *self);

#endif