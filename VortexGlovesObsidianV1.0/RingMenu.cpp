#include "RingMenu.h"

// menus
#include "GlobalBrightness.h"
#include "FactoryReset.h"
#include "ModeSharing.h"
#include "ColorSelect.h"
#include "PatternSelect.h"
#include "Randomizer.h"

#include "Button.h"

RingMenu::RingMenu() :
  m_selection(0),
  m_pCurMenu(nullptr),
  m_menuList()
{
}

bool RingMenu::init()
{
  // adding a menu consists of:
  //  name for sake of it
  //  menu object
  //  color
  
  // Create the randomizer and add it in first slot as white
  Randomizer *randomizer = new Randomizer(); 
  m_menuList.push_back(randomizer);

  // Create the color select and add it second as orange
  ColorSelect *colSelect = new ColorSelect();
  m_menuList.push_back(colSelect);

  // Create the pattern select
  PatternSelect *patSelect = new PatternSelect();
  m_menuList.push_back(patSelect);

  // create the global brightness menu
  GlobalBrightness *globBrightness = new GlobalBrightness();
  m_menuList.push_back(globBrightness);

  // createh factory reset menu
  FactoryReset *factReset = new FactoryReset();
  m_menuList.push_back(factReset);

  // create the mode sharing menu
  ModeSharing *modeShare = new ModeSharing();
  m_menuList.push_back(modeShare);

  return true;
}

const Menu *RingMenu::run(const Button *button, LedControl *ledControl)
{
  if (!button) {
    return NULL;
  }
  // show ring menu
  // the threshold for how long to hold to activate the menu
  int threshold = 1000 + (1000 * m_selection);
  if (button->holdDuration() < threshold) {
    return NULL;
  }
  // the ring menu is now open
  m_isOpen = true;
  // the amount of time held past the threshold
  int holdTime = (button->holdDuration() - threshold);
  // the leds turn on in sequence every 100ms another turns on:
  //  000ms = led 0 to 0
  //  100ms = led 0 to 1
  //  200ms = led 0 to 2
  int led = holdTime / 100;
  // only try to turn on 10 leds (0 through 9)
  if (led > 9) led = 9;
  // turn on leds 0 through led with hsv based on the menu section
  setLeds(0, led, m_menuList[m_selection].color());
  return NULL;
}
