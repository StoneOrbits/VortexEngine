#ifndef MENU_H
#define MENU_H

#include <inttypes.h>

#include "../ColorTypes.h"
#include "../LedConfig.h"

class Mode;

class Menu
{
public:
  Menu();
  virtual ~Menu();

  // optional init function can be overridden
  virtual bool init(Mode *curMode);

  // when the menu runs it will have access to time, the button and led control
  virtual bool run() = 0;

  // optional handlers for clicks
  virtual void onShortClick();
  virtual void onLongClick();

protected:
  // close the current menu
  void leaveMenu();
  // blink the selected finger
  void blinkSelection(uint32_t offMs = 350);

  // the current mode that was selected
  Mode *m_pCurMode;

  // all menus have a 'current selection' which can point at any finger
  Finger m_curSelection;

private:
  // whether to close the menu
  bool m_shouldClose;
};

#endif
