#include "RingMenu.h"

// menus
#include "GlobalBrightness.h"
#include "FactoryReset.h"
#include "ModeSharing.h"
#include "ColorSelect.h"
#include "PatternSelect.h"
#include "Randomizer.h"

#include "LedControl.h"
#include "Button.h"

RingMenu::RingMenu() :
  m_isOpen(false),
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

Menu *RingMenu::run(const Button *button, LedControl *ledControl)
{
  // basic sanity check
  if (!button || !ledControl) {
    // programmer error
    return NULL;
  }
  // if the button was released this tick and the ringmenu was open 
  // then close the ringmenu and return the current menu selection
  if (button->onRelease() && m_isOpen) {
    // the menu is no longer open
    m_isOpen = false;
    // return the menu that was selected
    return m_menuList[m_selection];
  }
  // make sure the button is pressed and held for at least one second
  if (!button->isPressed() || button->holdDuration() < 1000) {
    return NULL;
  }
  // the ring menu is now open
  m_isOpen = true;
  // this allows the menu to wrap around to beginning after the end
  // if the user never lets go of the button
  int holdDuration = button->holdDuration() % (1000 + (1000 * numMenus()));
  // the threshold for when the current menu starts 1sec + 1sec per menu
  int threshold = 1000 + (1000 * m_selection);
  // the amount of time held in the current menu, should be 0 to 1000 ms
  int holdTime = (holdDuration - threshold);
  // if the holdTime passes 1000 then iterate to next menu
  if (holdTime > 1000) {
    // increment selection and wrap around at numMenus
    m_selection = (m_selection + 1) % numMenus();
    // clear all the LEDs
    ledControl->clearAll();
    // re-calculate holdTime, should be 0-1000 now
    threshold = 1000 + (1000 * m_selection);
    holdTime = (holdDuration - threshold);
  }
  // the leds turn on in sequence every 100ms another turns on:
  //  000ms = led 0 to 0
  //  100ms = led 0 to 1
  //  200ms = led 0 to 2
  int led = holdTime / 100;
  // only try to turn on max num leds (0 through NUM_LEDS - 1)
  if (led > (NUM_LEDS - 1)) led = NUM_LEDS - 1;
  // turn on leds 0 through led with the menu's given color
  ledControl->setRange(0, led, m_menuList[m_selection]->color());
  return NULL;
}
