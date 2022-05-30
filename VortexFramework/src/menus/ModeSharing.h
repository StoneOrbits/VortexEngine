#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "Menu.h"

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
  enum class SharingMode {
    SHARE_SEND     = 0,
    SHARE_RECEIVE  = 1
  };

  SharingMode m_sharingMode;
};

#endif
