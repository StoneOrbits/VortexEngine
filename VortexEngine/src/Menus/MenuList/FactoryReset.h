#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include "../Menu.h"

class FactoryReset : public Menu
{
public:
  FactoryReset();
  ~FactoryReset();

  bool init() override;
  bool run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
};

#endif
