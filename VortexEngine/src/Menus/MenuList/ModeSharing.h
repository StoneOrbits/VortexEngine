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
  
  void showWaitMode();
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

  // the start time when waiting
  uint64_t m_waitStartTime;
  // the start time when checking for timing out
  uint64_t m_timeOutStartTime;
};

#endif
