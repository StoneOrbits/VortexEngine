#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/Mode.h"

class EditorConnection : public Menu
{
public:
  EditorConnection(const RGBColor &col, bool advanced);
  ~EditorConnection();

  bool init() override;
  MenuAction run() override;

  // broadcast the current preview mode over VL
  void sendCurModeVL();

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

  // menu conn
  void leaveMenu(bool doSave = false) override;

private:
  void showEditor();
  void receiveData();
  void sendModes();
  void sendModeCount();
  void sendCurMode();
  bool receiveModes();
  bool receiveModeCount();
  bool receiveMode();
  bool receiveDemoMode();
  void handleCommand();
  bool receiveMessage(const char *message);
  void clearDemo();

  enum EditorConnectionState {
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
  };

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
};

#endif
