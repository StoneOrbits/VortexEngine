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

  void showSendMode();
  void showReceiveMode();

  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  enum class ModeShareState {
    SHARE_SEND,     // send mode
    SHARE_RECEIVE,  // receive mode
  };

  ModeShareState m_sharingMode;
  // last time data was sent/received
  uint64_t m_lastAction;

  // IR sender blocks led Updates, begin send is for an extra tick to update the leds
  bool m_beginSend;
  // the start time when checking for timing out
  uint64_t m_timeOutStartTime;
};

#endif
