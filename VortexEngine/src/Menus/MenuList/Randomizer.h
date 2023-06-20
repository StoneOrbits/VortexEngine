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
  // random context for each led and led multi (LED_COUNT + 1)
  Random m_singlesRandCtx[LED_COUNT];
  Random m_multiRandCtx;

  // the time of the last randomization
  uint64_t m_lastRandomization;
  // whether auto-randomize is enabled
  bool m_autoMode;

  // re-roll a new randomization with a given context on an led
  bool reRoll(LedPos pos);
  bool reRoll();
};

#endif
