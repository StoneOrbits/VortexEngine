#include "MainMenu.h"

#include "../Time/TimeControl.h"
#include "../Storage/Storage.h"
#include "../Buttons/Buttons.h"
#include "../Leds/LedTypes.h"
#include "../Modes/Modes.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

bool MainMenu::m_isOpen = true;
uint8_t MainMenu::m_curSelection = 0;
bool MainMenu::m_loaded = false;
RGBColor MainMenu::m_profileColors[NUM_SELECTIONS] = {};

bool MainMenu::init()
{
  // main menus start open
  m_isOpen = true;
  m_curSelection = 0;
  // whether menu colors have been loaded
  m_loaded = false;
  // set default profile colors to begin
  setDefaultProfileColors();
  return true;
}

bool MainMenu::run()
{
  // if the main menus aren't open then nothing to do here
  if (!m_isOpen) {
    return false;
  }

  if (!m_loaded) {
    // if the header is corrupted or empty then it will fail to load
    if (Modes::loadHeader()) {
      m_loaded = true;
    }
    // loading save header might fail on first attempt when the header is null
    // or corrupted, loadHeader will in turn write out a new saveHeader and
    // this will run again and attempt to load it the second time
  }

  // press <
  if (g_pButtonL->onShortClick()) {
    pressLeft();
  }
  // press o
  if (g_pButtonM->onShortClick()) {
    select();
  }
  if (g_pButtonM->onLongClick()) {
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
  // render profile colors for each LED pair
  uint32_t now = Time::getCurtime();
  MAP_FOREACH_LED(MAP_OUTER_RING) {
    uint8_t profileIdx = (uint8_t)pos % NUM_SELECTIONS;
    RGBColor col = getProfileColor(profileIdx);
    Leds::breatheIndexRGB(pos, col, (now / 2), 8, 50);
  }
  MAP_FOREACH_LED(MAP_INNER_RING) {
    uint8_t profileIdx = (uint8_t)pos % NUM_SELECTIONS;
    RGBColor col = getProfileColor(profileIdx);
    Leds::breatheIndexRGB(pos, col, (now / 2), 8, 50);
  }
  Leds::blinkIndex((LedPos)m_curSelection);
  Leds::blinkIndex((LedPos)(m_curSelection + 10));
}

void MainMenu::open()
{
  m_isOpen = true;
}

void MainMenu::close()
{
  m_isOpen = false;
}

bool MainMenu::isOpen()
{
  return m_isOpen;
}

void MainMenu::pressLeft()
{
  if (!m_curSelection) {
    m_curSelection = NUM_SELECTIONS - 1;
  } else {
    m_curSelection--;
  }
}

void MainMenu::pressRight()
{
  m_curSelection = (m_curSelection + 1) % NUM_SELECTIONS;
}

void MainMenu::select()
{
  m_isOpen = false;
  Storage::setStoragePage(m_curSelection);
  if (!Modes::loadStorage()) {
    Modes::setDefaults();
  }
  DEBUG_LOGF("Selected storage page: %u", m_curSelection);
}

RGBColor MainMenu::getProfileColor(uint8_t index)
{
  if (index >= NUM_SELECTIONS) {
    return RGB_OFF;
  }
  return m_profileColors[index];
}

bool MainMenu::setProfileColor(uint8_t index, RGBColor color)
{
  if (index >= NUM_SELECTIONS) {
    return false;
  }
  m_profileColors[index] = color;
  return true;
}

void MainMenu::setDefaultProfileColors()
{
  m_profileColors[0]  = RGB_RED;
  m_profileColors[1]  = RGB_ORANGE;
  m_profileColors[2]  = RGB_YELLOW;
  m_profileColors[3]  = RGB_GREEN;
  m_profileColors[4]  = RGB_CYAN;
  m_profileColors[5]  = RGB_BLUE;
  m_profileColors[6]  = RGB_PURPLE;
  m_profileColors[7]  = RGB_MAGENTA;
  m_profileColors[8]  = RGB_PINK;
  m_profileColors[9]  = RGB_WHITE;
}
