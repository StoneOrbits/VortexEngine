#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include "../Menu.h"

class ModeSharing : public Menu
{
public:
  ModeSharing();
  ~ModeSharing();

  bool init() override;
  bool run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
};

#endif
