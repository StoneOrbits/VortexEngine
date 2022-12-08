#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"

class EditorConnection : public Menu
{
public:
  EditorConnection();

  bool init();
  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  void showEditor();
  void receiveData();
  void sendModes();
  void handleCommand();

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

    // Send the modes, then wait for the sent modes ack
    STATE_SEND_MODES,
    STATE_SEND_MODES_ACK,
  };

  // state of the editor
  EditorConnectionState m_state;
  // the data that is received
  ByteStream m_receiveBuffer;
};

#endif
