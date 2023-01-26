#include "Randomizer.h"

#include "../../Memory/Memory.h"

#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Modes/ModeBuilder.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

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
    ERROR_LOG("Failed to roll randomizer");
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

  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_DIM_WHITE);
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
  // we will need to save if the randomized mode is not equal to current mode
  bool needsSave = !m_pCurMode || !m_pCurMode->equals(m_pRandomizedMode);
  // update the current mode to be a copy of the randomized mode
  if (!Modes::updateCurMode(m_pRandomizedMode)) {
    ERROR_LOG("Failed to set randomized mode");
  } else {
    DEBUG_LOG("Saved new randomization");
  }
  // then done here, save if the mode was different
  leaveMenu(needsSave);
}

void Randomizer::onLongClick2()
{
  leaveMenu();
}

bool Randomizer::reRoll()
{
  // colorset that will be filled with random colors
  Colorset randomSet;
  // pick a random type of randomizer to use then use 
  // the randomizer to generate a random colorset
  uint32_t randType = random(0, 6);
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
    randomSet.randomizeTetradic();
    break;
  case 5:
    randomSet.randomizeEvenlySpaced();
    break;
  }
  // create a random pattern ID from all patterns
  PatternID randomPattern;
  do {
    // continuously re-randomize the pattern so we don't get solids
    randomPattern = (PatternID)random(PATTERN_FIRST, PATTERN_COUNT);
  } while (randomPattern == PATTERN_SOLID);

  if (!m_pRandomizedMode) {
    // create a new randomized mode out of the colors
    m_pRandomizedMode = ModeBuilder::make(randomPattern, nullptr, &randomSet);
    if (!m_pRandomizedMode) {
      return false;
    }
  } else {
    // set Randomized PatternID and color set
    m_pRandomizedMode->setPattern(randomPattern);
    m_pRandomizedMode->setColorset(&randomSet);
  }
  m_pRandomizedMode->init();

  DEBUG_LOGF("Randomized set with randomization technique %u, %u colors, and Pattern number %u",
    randType, randomSet.numColors(), randomPattern);
  return true;
}

// override showExit so it isn't displayed on thumb
void Randomizer::showExit()
{
}