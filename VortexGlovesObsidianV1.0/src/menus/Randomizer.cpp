#include "Randomizer.h"

#include "../LedControl.h"
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
  // TODO: save randomization

  // then do default actions leave menu
  Menu::onLongClick();
}

//TODO: remove these includes if possible when randomization is fixed
#include "../patterns/BasicPattern.h"
#include "../Colorset.h"

bool Randomizer::reRoll()
{
  if (m_pRandomizedMode) {
    delete m_pRandomizedMode;
  }
  m_pRandomizedMode = new Mode();

  // TODO: properly do this
  BasicPattern *strobe = new BasicPattern(5, 8);
  Colorset *rgb = new Colorset(0xFF0000, 0xFF00, 0xFF);
  Mode *rgbStrobe = new Mode();
  rgbStrobe->bindAll(strobe, rgb);

  m_pRandomizedMode = rgbStrobe;

  if (!m_pRandomizedMode) {
    return false;
  }

  return true;
}
