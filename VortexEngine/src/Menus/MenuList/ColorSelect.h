#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"

class ColorSelect : public Menu
{
public:
  ColorSelect(const RGBColor &col);
  ~ColorSelect();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  //void onShortClick() override;
  //void onLongClick() override;

private:
};

#endif
