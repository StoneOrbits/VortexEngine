#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/Mode.h"

class EditorConnection : public Menu
{
public:
  EditorConnection(VortexEngine &engine, const RGBColor &col, bool advanced);
  ~EditorConnection();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

  // menu conn
  void leaveMenu(bool doSave = false) override;

private:
  void showEditor();
  void receiveData();
  void sendModes();
  bool receiveModes();
  bool receiveDemoMode();
  void handleCommand();
  bool receiveMessage(const char *message);
  void clearDemo();

  enum EditorConnectionState {
    // the editor is not connec
    STATE_DISCONNECTED,

    // Sending the greeting message
    STATE_GREETING,

    // entirely idle, waiting for commands
    STATE_IDLE,

    // engine pulls the modes from gloves, then wait for the sent modes ack
    STATE_PULL_MODES,
    STATE_PULL_MODES_SEND,
    STATE_PULL_MODES_DONE,

    // engine pushes modes to gloves, then waits for done
    STATE_PUSH_MODES,
    STATE_PUSH_MODES_RECEIVE,
    STATE_PUSH_MODES_DONE,

    // engine pushes mode to gloves for demo while idle
    STATE_DEMO_MODE,
    STATE_DEMO_MODE_RECEIVE,
    STATE_DEMO_MODE_DONE,

    // engine tells gloves to clear the demo preview, gloves acknowledge
    STATE_CLEAR_DEMO,
  };

  // state of the editor
  EditorConnectionState m_state;
  // the data that is received
  ByteStream m_receiveBuffer;
};

#endif
