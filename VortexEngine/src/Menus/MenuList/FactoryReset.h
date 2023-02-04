#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include "../Menu.h"

class FactoryReset : public Menu
{
public:
  FactoryReset();

  bool init() override;

  bool run() override;

  // handlers for clicks
  void onShortClick() override;
  void onShortClick2() override;
  void onLongClick() override;
  void onLongClick2() override;

private:
  // show the reset lights
  void showReset();
};

#endif
