#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "Menu.h"

class ModeSharing : public Menu
{
  public:
    ModeSharing();

    bool init(Mode *curMode);

    bool run();

    // handlers for clicks
    void onShortClick();
    void onLongClick();

  private:

};

#endif
