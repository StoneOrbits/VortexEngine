#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "../Menu.h"

class ModeSharing : public Menu
{
public:
  ModeSharing(const RGBColor &col, bool advanced);
  ~ModeSharing();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  void beginSendingVL();
  void continueSendingVL();
  void beginReceivingVL();
  void receiveModeVL();

  void showSendModeVL();
  void showReceiveMode();

  enum class ModeShareState {
    SHARE_SEND_VL,  // send mode over vl
    SHARE_RECEIVE,  // receive mode
  };

  ModeShareState m_sharingMode;

  // the start time when checking for timing out
  uint32_t m_timeOutStartTime;
};

#endif
