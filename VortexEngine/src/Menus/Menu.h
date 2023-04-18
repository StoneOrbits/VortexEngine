#ifndef MENU_H
#define MENU_H

#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"

class Mode;

class Menu
{
public:
  Menu(const RGBColor &col);
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

  // optional handlers for clicks
  virtual void onShortClick();
  virtual void onShortClick2();
  virtual void onLongClick();
  virtual void onLongClick2();

  // close the current menu
  virtual void leaveMenu(bool doSave = false);

protected:
  void showBulbSelection();
  void showSelect();
  void showExit();

  // blink the selected finger
  virtual void blinkSelection(uint32_t offMs = 250, uint32_t onMs = 500);

  // the current mode that was selected
  Mode *m_pCurMode;
  // the color of this menu
  RGBColor m_menuColor;
  // tracks the targetted leds for this menu
  // note this is an led map
  LedMap m_targetLeds;
  // true once a an led is selected
  bool m_ledSelected;
  // all menus have a 'current selection which can point at any led
  Quadrant m_curSelection;

private:
  // internal flag to close the menu
  bool m_shouldClose;
};

#endif
