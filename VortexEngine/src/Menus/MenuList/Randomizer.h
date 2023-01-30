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

  bool init();

  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  // a demo mode for the current randomization
  Mode m_demoMode;

  // re-roll a new randomization
  bool reRoll();
};

#endif
