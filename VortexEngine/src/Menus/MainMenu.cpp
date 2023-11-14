#include "MainMenu.h"

#include "../Time/TimeControl.h"
#include "../Buttons/Buttons.h"
#include "../Leds/LedTypes.h"
#include "../Leds/Leds.h"

bool MainMenu::m_isOpen = false;
uint8_t MainMenu::m_curSelection = 0;

#define NUM_SELECTIONS (LED_COUNT / 2)

bool MainMenu::init()
{
  // main menus start open
  m_isOpen = true;
  m_curSelection = 0;
  return true;
}

bool MainMenu::run()
{
  // if the main menus aren't open then nothing to do here
  if (!m_isOpen) {
    return false;
  }

  // press <
  if (g_pButtonL->onShortClick()) {
    pressLeft();
  }
  // press o
  if (g_pButtonM->onShortClick()) {
    select();
  }
  // press >
  if (g_pButtonR->onShortClick()) {
    pressRight();
  }

  // render
  show();
  return true;
}

void MainMenu::show()
{
  Leds::clearAll();
  // render the main menu
  uint8_t hue = 0;
  uint32_t now = Time::getCurtime();
  MAP_FOREACH_LED(MAP_OUTER_RING) {
    Leds::breathIndex(pos, hue, (now / 2), 8, 255, 180);
    hue -= (255 / (LED_COUNT / 2));
  }
  hue = 0;
  MAP_FOREACH_LED(MAP_INNER_RING) {
    Leds::breathIndex(pos, hue, (now / 2), 8, 255, 180);
    hue -= (255 / (LED_COUNT / 2));
  }
  Leds::blinkIndex((LedPos)m_curSelection);
  Leds::blinkIndex((LedPos)(m_curSelection + 10));
}

void MainMenu::open()
{
  m_isOpen = true;
}

bool MainMenu::isOpen()
{
  return m_isOpen;
}

void MainMenu::pressLeft()
{
  m_curSelection = (m_curSelection + 1) % NUM_SELECTIONS;
}

void MainMenu::pressRight()
{
  if (!m_curSelection) {
    m_curSelection = NUM_SELECTIONS - 1;
  } else {
    m_curSelection--;
  }
}

void MainMenu::select()
{
  m_isOpen = false;
}
