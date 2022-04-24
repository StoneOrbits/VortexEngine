#include "Randomizer.h"

#include "../ModeBuilder.h"
#include "../LedControl.h"
#include "../Settings.h"
#include "../Button.h"
#include "../Mode.h"

Randomizer::Randomizer() :
  Menu(),
  m_pRandomizedMode(nullptr)
{
}

bool Randomizer::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  // re-roll the randomization
  if (!reRoll()) {
    // fatal error
    return false;
  }
  return true;
}

bool Randomizer::run()
{
  // run the base menu logic
  if (!Menu::run()) {
    return false;
  }

  // display the randomized mode
  m_pRandomizedMode->play();

  // return true to continue staying in randomizer menu
  return true;
}


void Randomizer::onShortClick()
{
  // shortClick re-roll the randomization
  if (!reRoll()) {
    // fatal error
  }
}

void Randomizer::onLongClick()
{
  // replace the current mode with randomized one
  if (!g_pSettings->setCurMode(m_pRandomizedMode)) {
    // error
  } 
  // then do default actions leave menu
  Menu::onLongClick();
}

bool Randomizer::reRoll()
{
  if (m_pRandomizedMode) {
    delete m_pRandomizedMode;
  }

  // pick a random pattern
  PatternID pattern = (PatternID)random(PATTERN_FIRST, PATTERN_LAST);

  // pick a random amount of colors
  uint32_t numColors = random(2,8);

  // fill the array with up to numColors random colors
  RGBColor c[8] = { RGB_OFF };
  for (uint32_t i = 0; i < numColors; ++i) {
    c[i] = RGBColor(random(0,255), random(0,255), random(0, 255));
  }

  // create a new randomized mode out of the colors
  m_pRandomizedMode = ModeBuilder::make(pattern, 
      c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);

  if (!m_pRandomizedMode) {
    return false;
  }

  return true;
}
