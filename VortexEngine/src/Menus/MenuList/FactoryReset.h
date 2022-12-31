#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include "../Menu.h"

class FactoryReset : public Menu
{
public:
  FactoryReset();

  bool init();

  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  // show the reset lights
  void showReset();
};

#endif
