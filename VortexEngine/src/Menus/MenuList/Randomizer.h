#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include "../Menu.h"

#include "../../Random/Random.h"
#include "../../Modes/Mode.h"

class Mode;

class Randomizer : public Menu
{
public:
  Randomizer(const RGBColor &col);
  ~Randomizer();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

private:
  // random context for each led
  Random m_randCtx[LED_COUNT];

  // re-roll a new randomization with a given context on an led
  bool reRoll(LedPos led, Random &ctx);
  void reRoll();
};

#endif