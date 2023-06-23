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
  void receiveMode();

  void showSendModeVL();
  void showSendModeIR();
  void showReceiveMode();

  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  enum class ModeShareState {
    SHARE_SEND_IR,  // send mode over ir
    SHARE_SEND_VL,  // send mode over vl
    SHARE_RECEIVE,  // receive mode
  };

  ModeShareState m_sharingMode;
  // last time data was sent
  uint64_t m_lastActionTime;

  // the start time when checking for timing out
  uint64_t m_timeOutStartTime;
};

#endif
