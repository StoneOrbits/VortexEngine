#include "Randomizer.h"

#include "../ModeBuilder.h"
#include "../LedControl.h"
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
  // replace the current mode with randomized one
  if (!Modes::setCurMode(m_pRandomizedMode)) {
    // error
  }
  m_pRandomizedMode = nullptr;
  DEBUG("Saved new randomization");
  // then done here
  leaveMenu();
}

bool Randomizer::reRoll()
{
  if (m_pRandomizedMode) {
    delete m_pRandomizedMode;
    m_pRandomizedMode = nullptr;
  }

  // pick a random pattern
  PatternID pattern = PATTERN_SINGLE_FIRST;//(PatternID)random(PATTERN_SINGLE_FIRST, PATTERN_SINGLE_LAST);

  // pick a random amount of colors
  uint32_t numColors = 3;//random(2, 7);

  // fill the array with up to numColors random colors
  RGBColor c[8] = { RGB_OFF };
  for (uint32_t i = 0; i < numColors; ++i) {
    c[i] = RGBColor(
      (uint8_t)random(0, 255),
      (uint8_t)random(0, 255),
      (uint8_t)random(0, 255));
  }

  // create a new randomized mode out of the colors
  m_pRandomizedMode = ModeBuilder::make(pattern,
    c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);

  if (!m_pRandomizedMode) {
    return false;
  }

  m_pRandomizedMode->init();

  return true;
}
