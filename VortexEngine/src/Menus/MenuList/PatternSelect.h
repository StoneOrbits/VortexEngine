#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"
#include "../../Patterns/Patterns.h"
#include "../../Modes/Mode.h"

class PatternSelect : public Menu
{
public:
  PatternSelect(const RGBColor &col);
  ~PatternSelect();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
};

#endif
