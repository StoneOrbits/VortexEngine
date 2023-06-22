#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "../Menu.h"

class ModeSharing : public Menu
{
public:
  ModeSharing(const RGBColor &col);
  ~ModeSharing();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  void beginSending();
  void continueSending();
  void beginReceiving();
  void receiveMode();

  void showSendMode();
  void showReceiveMode();

  enum class ModeShareState {
    SHARE_SEND,     // send mode
    SHARE_RECEIVE,  // receive mode
  };

  ModeShareState m_sharingMode;

  // the start time when checking for timing out
  uint64_t m_timeOutStartTime;
};

#endif
