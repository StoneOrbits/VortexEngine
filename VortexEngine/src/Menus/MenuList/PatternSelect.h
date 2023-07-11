#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"
#include "../../Modes/Mode.h"

class PatternSelect : public Menu
{
public:
  PatternSelect(const RGBColor &col, bool advanced);
  ~PatternSelect();

  bool init() override;
  MenuAction run() override;

  // callback after the user selects the target led
  void onLedSelected() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  // helpful member
  LedPos m_srcLed;

  // used for adv pat select
  uint8_t m_argIndex;
};

#endif
