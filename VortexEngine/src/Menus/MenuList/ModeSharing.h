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
  void onShortClick2();
  void onLongClick();
  void onLongClick2();

private:
  void beginSending();
  void continueSending();
  void receiveMode();

  void showSendMode();
  void showReceiveMode();

  enum class ModeShareState {
    SHARE_SEND,     // send mode
    SHARE_RECEIVE,  // receive mode
  };

  ModeShareState m_sharingMode;
  // last time data was sent
  uint64_t m_lastActionTime;

  // the start time when checking for timing out
  uint64_t m_timeOutStartTime;
};

#endif
