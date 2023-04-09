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
  virtual void onLongClick();

  // close the current menu
  virtual void leaveMenu(bool doSave = false);

protected:
  void showBulbSelection();

  // the current mode that was selected
  Mode *m_pCurMode;
  // the color of this menu
  RGBColor m_menuColor;
  // tracks the current selected led
  LedPos m_targetLed;
  // true once a an led is selected
  bool m_ledSelected;

private:
  // internal flag to close the menu
  bool m_shouldClose;
};

#endif
