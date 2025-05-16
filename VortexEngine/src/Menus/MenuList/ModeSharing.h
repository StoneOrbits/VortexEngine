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

  // callback after the user selects the target led
  void onLedSelected() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  void receiveMode();
  void showReceiveMode();

  enum class ModeShareState {
    // these three are the main options that can be iterated through
    // and if the button is held it will send, otherwise receive
    SHARE_SEND_RECEIVE,           // send/receive mode
    SHARE_SEND_RECEIVE_LEGACY,    // send/receive mode legacy
    SHARE_EXIT,                   // exit mode sharing
  };

  ModeShareState m_sharingMode;

  // the start time when checking for timing out
  uint32_t m_timeOutStartTime;

  // used to track when the receive percentage changes for timeout purposes
  uint32_t m_lastPercentChange;
  uint8_t m_lastPercent;
};

#endif
