#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"

class ColorSelect : public Menu
{
public:
  ColorSelect();
  ~ColorSelect();

  bool init() override;
  bool run() override;

  // handlers for clicks
  //void onShortClick() override;
  //void onLongClick() override;

private:
};

#endif
