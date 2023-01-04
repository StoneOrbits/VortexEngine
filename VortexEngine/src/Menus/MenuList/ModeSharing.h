#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "../Menu.h"

class ModeSharing : public Menu
{
public:
  ModeSharing();

  bool init();
  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  void beginSending();
  void continueSending();
  void receiveMode();
  
  void showSendBreak();
  void showSendMode();
  void showReceiveMode();

  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  enum class ModeShareState {
    SHARE_SEND,     // send mode
    SHARE_RECEIVE,  // receive mode
    SHARE_WAIT,     // wait before sending again
  };

  ModeShareState m_sharingMode;
  // last time data was sent/received
  uint64_t m_last_action;

  uint64_t m_breakStartTime;
};

#endif
