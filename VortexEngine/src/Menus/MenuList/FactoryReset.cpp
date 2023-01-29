#include "FactoryReset.h"

#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

FactoryReset::FactoryReset() :
  Menu()
{
}

bool FactoryReset::init()
{
  if (!Menu::init()) {
    return false;
  }
  // start on exit by default
  m_curSelection = FINGER_THUMB;
  DEBUG_LOG("Entered factory reset");
  return true;
}

bool FactoryReset::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // show the reset menu
  showReset();

  // blink the selection
  blinkSelection();

  // continue
  return true;
}

void FactoryReset::onShortClick()
{
  if (m_curSelection == FINGER_THUMB) {
    m_curSelection = FINGER_COUNT;
  } else {
    m_curSelection = FINGER_THUMB;
  }
}

void FactoryReset::onLongClick()
{
  switch (m_curSelection) {
  case FINGER_THUMB:
    leaveMenu();
    return;
  case FINGER_COUNT:
    // must wait till all empty + one white blink
    if (g_pButton->holdDuration() > (FACTORY_RESET_THRESHOLD_TICKS + Time::msToTicks(700))) {
      Modes::setDefaults();
      leaveMenu(true);
    }
    break;
  }
}

void FactoryReset::showReset()
{
  // if we're on the thumb just set the rest to blank
  if (m_curSelection == FINGER_THUMB) {
    Leds::setRange(LED_FIRST, INDEX_TOP, RGB_BLANK);
    return;
  }
  // otherwise we're not on thumb, if the button isn't pressed
  if (!g_pButton->isPressed()) {
    // just idle blink from clear to blank
    Leds::clearRange(LED_FIRST, INDEX_TOP);
    Leds::blinkRange(LED_FIRST, INDEX_TOP, Time::getCurtime(), 250, 150, RGB_BLANK);
    return;
  }

  // the button is pressed so show the reset countdown timer

  // the progress is how long the hold duration has been held
  // relative to the factory reset threshold time
  float progress = (float)g_pButton->holdDuration() / FACTORY_RESET_THRESHOLD_TICKS;
  // the ledProgress is just an LED from pinky tip to index top based on progress
  LedPos ledProgress = (LedPos)(progress * INDEX_TOP);
  // max the led progress at index top (don't include thumb)
  if (ledProgress > INDEX_TOP) {
    // when we reach the end of the progress bar just blink white
    Leds::blinkRange(LED_FIRST, INDEX_TOP, Time::getCurtime(), 80, 60, RGB_DIM_WHITE1);
    return;
  }

  // the off/on ms blink faster based on the progress
  uint32_t offMs = 150 - (12 * ledProgress);
  uint32_t onMs = 200 - (10 * ledProgress);
  // the hue gets more red starting at 20 going in steps of 4 to min 0
  int8_t hue = (ledProgress < 6) ? 20 - (4 * ledProgress) : 0;
  // the 'endled' is the tip of the reset progress bar, since the progress
  // bar starts full red and empties down to the pinky that means it is
  // inverted from the 'ledProgress' which starts at 0 and grows
  LedPos endLed = (LedPos)(INDEX_TOP - ledProgress);
  // clear all the leds so that 'blinkRange' will blink from off to the designated color
  Leds::clearRange(LED_FIRST, INDEX_TOP);
  // blink to the calculated redish hue from pinky to the end led
  Leds::blinkRange(LED_FIRST, endLed, Time::getCurtime(), offMs, onMs, HSVColor(hue, 255, 180));
  // and blink the background the regular blank color
  Leds::blinkRange((LedPos)(endLed + 1), INDEX_TOP, Time::getCurtime(), offMs, onMs, RGB_BLANK);
}
