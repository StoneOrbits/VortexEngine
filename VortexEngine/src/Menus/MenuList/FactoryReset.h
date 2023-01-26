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
  void onShortClick2();
  void onLongClick();
  void onLongClick2();

private:
  // show the reset lights
  void showReset();
};

#endif
