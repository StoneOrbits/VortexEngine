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

    // Sending the hello verb, then waiting for the hello ack verb
    STATE_HELLO,
    STATE_HELLO_ACK,

    // send the idle verb, then wait for the idle ack
    STATE_SEND_IDLE,
    STATE_IDLE_ACK,

    // entirely idle, waiting for commands
    STATE_IDLE,

    // engine pulls the modes from gloves, then wait for the sent modes ack
    STATE_PULL_MODES,
    STATE_PULL_MODES_ACK,

    // engine pushes modes to gloves, then waits for done
    STATE_PUSH_MODES,
    STATE_PUSH_MODES_RECEIVE,
    STATE_PUSH_MODES_DONE,

    // engine pushes mode to gloves for demo while idle
    STATE_DEMO_MODE,
    STATE_DEMO_MODE_RECEIVE,
    STATE_DEMO_MODE_DONE,
  };

  // state of the editor
  EditorConnectionState m_state;
  // the data that is received
  ByteStream m_receiveBuffer;
  // the mode to demo if there is one
  Mode *m_pDemoMode;
};

#endif
