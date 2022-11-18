#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "../Menu.h"

// sender is a stream based object, receiving is passive
#include "../../Infrared/IRSender.h"

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
  void sendMode();
  void receiveMode();

  void showSendMode();
  void showReceiveMode();

  enum class ModeShareState {
    SHARE_SEND,     // send mode
    SHARE_RECEIVE,  // receive mode
  };

  ModeShareState m_sharingMode;
  // last time data was sent/received
  uint64_t m_last_action;
  // The ir sender context
  IRSender m_irSender;
};

#endif
