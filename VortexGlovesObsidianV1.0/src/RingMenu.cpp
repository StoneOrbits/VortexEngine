#include "RingMenu.h"

// menus
#include "menus/GlobalBrightness.h"
#include "menus/FactoryReset.h"
#include "menus/ModeSharing.h"
#include "menus/ColorSelect.h"
#include "menus/PatternSelect.h"
#include "menus/Randomizer.h"

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
    return nullptr;
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
    return nullptr;
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
  LedPos led = (LedPos)(holdTime / 100);
  // only try to turn on max num leds (LED_FIRST through LED_LAST)
  if (led > LED_LAST) led = LED_LAST;
  // turn on leds LED_FIRST through led with the menu's given color
  ledControl->setRange(LED_FIRST, led, m_menuList[m_selection]->color());
  return nullptr;
}
