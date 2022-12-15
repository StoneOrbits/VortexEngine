#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"

class EditorConnection : public Menu
{
public:
  EditorConnection();
  ~EditorConnection();

  bool init();
  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  void showEditor();
  void receiveData();
  void sendModes();
  bool receiveModes();
  bool receiveDemoMode();
  void handleCommand();
  bool receiveMessage(const char *message);

  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  enum EditorConnectionState {
    // the editor is not connec
    STATE_DISCONNECTED,

    // Sending the greeting message
    STATE_GREETING,

    // Waiting for the hello, then responding with the ack
    STATE_HELLO,
    STATE_HELLO_DONE,

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
  // the mode to demo if there is one
  Mode *m_pDemoMode;
};

#endif
