#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H


#include "../Menu.h"

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
  void receiveMessage();

  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  enum EditorConnectionState {
    STATE_DISCONNECTED,

    STATE_HELLO,
    STATE_HELLO_RECEIVE,

    STATE_IDLE,
  };

  // state of the editor
  EditorConnectionState m_state;
};

#endif
