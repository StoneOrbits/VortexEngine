#ifndef MAINMENU_H
#define MAINMENU_H

#include <inttypes.h>
#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"

#define NUM_SELECTIONS (LED_COUNT / 2)

class MainMenu
{
public:
  static bool init();
  static bool run();
  static void show();

  // open the main menu
  static void open();
  static void close();
  static bool isOpen();
  static void pressLeft();
  static void pressRight();
  static void select();

  // profile colors for main menu display
  static RGBColor getProfileColor(uint8_t index);
  static bool setProfileColor(uint8_t index, RGBColor color);
  static void setDefaultProfileColors();
  static RGBColor m_profileColors[NUM_SELECTIONS];

private:
  static bool m_isOpen;
  static uint8_t m_curSelection;
};

#endif
