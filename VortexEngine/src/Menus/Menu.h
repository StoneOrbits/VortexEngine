#ifndef MENU_H
#define MENU_H

#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"

class Mode;

class Menu
{
public:
  Menu(const RGBColor &col, bool advanced);
  virtual ~Menu();

  virtual bool init();

  // the action for the menu to execute
  enum MenuAction :uint8_t {
    // quit the menus
    MENU_QUIT,
    // continue running the menu
    MENU_CONTINUE,
    // don't run derived menu code, this is used internally
    // by the menu class itself
    MENU_SKIP
  };
  virtual MenuAction run();

  // optional handler for initializing data after user selected led
  virtual void onLedSelected();

  // optional handlers for clicks
  virtual void onShortClick();
  virtual void onLongClick();

  // close the current menu
  virtual void leaveMenu(bool doSave = false);

protected:
  void showBulbSelection();
  void showSelect();
  virtual void showExit();

  // blink the selected finger
  virtual void blinkSelection(uint32_t offMs = 250, uint32_t onMs = 500);

  // iterate to next bulb selection
  void nextBulbSelection();

  // the current mode that was selected
  Mode *m_pCurMode;
  // the color of this menu
  RGBColor m_menuColor;
  // tracks the targetted leds for this menu
  // note this is an led map
  LedMap m_targetLeds;
  // all menus have a 'current selection' which can point at any finger
  Finger m_curSelection;
  // true once a an led is selected
  bool m_ledSelected;
  // whether advanced menu was activated
  bool m_advanced;

private:
  // internal flag to close the menu
  bool m_shouldClose;
};

#endif
