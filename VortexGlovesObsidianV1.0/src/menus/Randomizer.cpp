#include "Randomizer.h"

#include "../patterns/Pattern.h"
#include "../ModeBuilder.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Timings.h"
#include "../Button.h"
#include "../Modes.h"
#include "../Mode.h"
#include "../Log.h"

Randomizer::Randomizer() :
  Menu(),
  m_pRandomizedMode(nullptr)
{
}

bool Randomizer::init()
{
  if (!Menu::init()) {
    return false;
  }
  PatternID id = m_pCurMode->getPatternID();
  DEBUGF("Cur pattern id: %u", id);
  if (!m_pRandomizedMode) {
    // create a new randomized mode out of the colors
    m_pRandomizedMode = ModeBuilder::make(id, m_pCurMode->getColorset());
    if (!m_pRandomizedMode) {
      return false;
    }
  }
  // re-roll the randomization
  if (!reRoll()) {
    // fatal error
    return false;
  }
  DEBUG("Entered randomizer");
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

  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD) {
    Leds::setAll(RGB_GREEN);
  }

  // return true to continue staying in randomizer menu
  return true;
}

void Randomizer::onShortClick()
{
  // shortClick re-roll the randomization
  if (!reRoll()) {
    // fatal error
  }
  DEBUG("Re-rolling randomization");
}

void Randomizer::onLongClick()
{
  // update the current mode to be a copy of the randomized mode
  if (!Modes::setCurMode(m_pRandomizedMode)) {
    // error
  }
  DEBUG("Saved new randomization");
  // then done here
  leaveMenu();
}

bool Randomizer::reRoll()
{
  // pick a random amount of colors
  uint32_t numColors = 3;//random(2, 7);

  // fill the array with up to numColors random colors
  Colorset randomSet;
  for (uint32_t i = 0; i < numColors; ++i) {
    randomSet.addColor(RGBColor((uint8_t)random(0, 255),
                                (uint8_t)random(0, 255),
                                (uint8_t)random(0, 255)));
  }

  m_pRandomizedMode->setColorset(&randomSet);
  m_pRandomizedMode->init();

  return true;
}
