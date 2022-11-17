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
  // whether to confirm the factory reset
  bool m_confirm;
};

#endif
