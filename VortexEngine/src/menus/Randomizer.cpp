#include "Randomizer.h"

#include "../Memory.h"

#include "../patterns/Pattern.h"
#include "../ModeBuilder.h"
#include "../Colorset.h"
#include "../Timings.h"
#include "../Button.h"
#include "../Modes.h"
#include "../Mode.h"
#include "../Leds.h"
#include "../Log.h"

Randomizer::Randomizer() :
  Menu(),
  m_pRandomizedMode(nullptr)
{
}

Randomizer::~Randomizer()
{
  if (m_pRandomizedMode) {
    delete m_pRandomizedMode;
    m_pRandomizedMode = nullptr;
  }
}

bool Randomizer::init()
{
  if (!Menu::init()) {
    return false;
  }
  // re-roll the randomization
  if (!reRoll()) {
    ERROR_LOG("Failed to re-roll randomizer");
    return false;
  }
  DEBUG_LOG("Entered randomizer");
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
    ERROR_LOG("Failed to re-roll randomizer");
  } else {
    DEBUG_LOG("Re-rolling randomization");
  }
}

void Randomizer::onLongClick()
{
  // update the current mode to be a copy of the randomized mode
  if (!Modes::setCurMode(m_pRandomizedMode)) {
    ERROR_LOG("Failed to set randomized mode");
  } else {
    DEBUG_LOG("Saved new randomization");
  }
  // then done here
  leaveMenu();
}

bool Randomizer::reRoll()
{
  // colorset that will be filled with random colors
  Colorset randomSet;
  // pick a random type of randomizer to use then use 
  // the randomizer to generate a random colorset
  uint32_t randType = random(0, 7);
  switch (randType) {
  default:
  case 0:
    randomSet.randomize();
    break;
  case 1:
    randomSet.randomizeColorTheory();
    break;
  case 2:
    randomSet.randomizeMonochromatic();
    break;
  case 3:
    randomSet.randomizeDoubleSplitComplimentary();
    break;
  case 4:
    randomSet.randomizeDoubleSplitComplimentary();
    break;
  case 5:
    randomSet.randomizeTetradic();
    break;
  case 6:
    randomSet.randomizeEvenlySpaced();
    break;
  }
  if (!m_pRandomizedMode) {
    if (!m_pCurMode) {
      return false;
    }
    // create a new randomized mode out of the colors
    m_pRandomizedMode = ModeBuilder::make(m_pCurMode->getPatternID(), &randomSet);
    if (!m_pRandomizedMode) {
      return false;
    }
  } else {
    m_pRandomizedMode->setColorset(&randomSet);
  }
  m_pRandomizedMode->init();

  DEBUG_LOGF("Randomized set with randomization technique %u and %u colors",
    randType, randomSet.numColors());

  return true;
}
