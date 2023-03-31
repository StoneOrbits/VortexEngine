#ifndef MENU_H
#define MENU_H

#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"

class Mode;

class Menu
{
public:
  Menu();
  virtual ~Menu();

  // optional init function can be overridden, this is called when the menu is
  // first opened to initialize the menu. It will be called each time the menu
  // is opened
  virtual bool init();

  // when the menu runs it will have access to time, the button and led control
  virtual bool run() = 0;

  // optional handlers for clicks
  virtual void onShortClick();
  virtual void onLongClick();

  // close the current menu
  virtual void leaveMenu(bool doSave = false);

protected:
  // the current mode that was selected
  Mode *m_pCurMode;

private:
  // whether to close the menu
  bool m_shouldClose;
};

#endif
