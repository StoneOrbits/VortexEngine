#ifndef MAINMENU_H
#define MAINMENU_H

#include <inttypes.h>

class MainMenu
{
public:
  static bool init();
  static bool run();
  static void show();
private:
  static void pressLeft();
  static void pressRight();
  static void select();

  static bool m_isOpen;
  static uint8_t m_curSelection;
};

#endif
