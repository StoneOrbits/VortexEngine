#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "../Menu.h"
#include "../../Modes/Mode.h"

class Mode;

class Randomizer : public Menu
{
public:
  Randomizer();
  ~Randomizer();

  bool init() override;

  bool run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;
  void onLongClick2() override;

private:
  // a demo mode for the current randomization
  Mode m_demoMode;

  // re-roll a new randomization
  bool reRoll();
};

#endif
